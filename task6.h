#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <cmath>
#include <stdexcept>
#include <string>
#include <algorithm>

namespace graph
{
    const double _INF = INFINITY;

    using Weight = double;
    using Vertex = size_t;
    using AdjacencyMatrix = std::vector<std::vector<Weight>>;

    class Graph
    {
    public:
        Graph() : adjacency_matrix_() {}
        Graph(size_t vertex_count) : adjacency_matrix_(vertex_count, std::vector<Weight>(vertex_count, _INF)) {}
        Graph(const Graph &rhs) : adjacency_matrix_(rhs.adjacency_matrix_) {}
        Graph(Graph &&rhs) noexcept : adjacency_matrix_(std::move(rhs.adjacency_matrix_)) {}

        size_t size() const { return adjacency_matrix_.size(); }
        bool empty() const { return !size(); }

        void resize(size_t new_vertex_count)
        {
            adjacency_matrix_.clear();
            adjacency_matrix_.resize(new_vertex_count, std::vector<Weight>(new_vertex_count, _INF));
        }

        const std::vector<Weight> &GetAdjacents(Vertex v) const
        {
            // Так-то at() ошибку перехватит, просто попытка в хороший тон
            if (v >= adjacency_matrix_.size())
            {
                std::string error = "There is no vertex with index " + std::to_string(v);
                throw std::out_of_range(error);
            }
            return adjacency_matrix_.at(v);
        }

        void ChangeWeight(Vertex from, Vertex to, Weight w)
        {
            if (from >= adjacency_matrix_.size())
            {
                std::string error = "There is no vertex with index " + std::to_string(from);
                throw std::out_of_range(error);
            }
            else if (to >= adjacency_matrix_.size())
            {
                std::string error = "There is no vertex with index " + std::to_string(to);
                throw std::out_of_range(error);
            }
            adjacency_matrix_.at(from).at(to) = w;
        }

        Weight GetWeight(Vertex from, Vertex to) const
        {
            if (from >= adjacency_matrix_.size())
            {
                std::string error = "There is no vertex with index " + std::to_string(from);
                throw std::out_of_range(error);
            }
            else if (to >= adjacency_matrix_.size())
            {
                std::string error = "There is no vertex with index " + std::to_string(to);
                throw std::out_of_range(error);
            }
            return adjacency_matrix_.at(from).at(to);
        }

        std::pair<AdjacencyMatrix, AdjacencyMatrix> WFI(AdjacencyMatrix adj_mat)
        {
            size_t vertex_count = adj_mat.size();
            AdjacencyMatrix next(vertex_count, std::vector<Weight>(vertex_count, _INF));

            for (size_t i = 0; i < vertex_count; ++i)
            {
                for (size_t j = 0; j < vertex_count; ++j)
                {
                    if (i == j || adj_mat[i][j] < _INF)
                    {
                        next[i][j] = j;
                    }
                }
            }

            for (size_t i = 0; i < vertex_count; ++i)
            {
                for (size_t u = 0; u < vertex_count; ++u)
                {
                    for (size_t v = 0; v < vertex_count; ++v)
                    {
                        if (adj_mat[u][v] > adj_mat[u][i] + adj_mat[i][v])
                        {
                            adj_mat[u][v] = adj_mat[u][i] + adj_mat[i][v];
                            next[u][v] = next[u][i];
                        }
                    }
                }
            }
            return {adj_mat, next};
        }

        std::vector<Vertex> GetShortestPath(const AdjacencyMatrix &next, Vertex from, Vertex to)
        {
            if (from >= adjacency_matrix_.size())
            {
                std::string error = "There is no vertex with index " + std::to_string(from);
                throw std::out_of_range(error);
            }
            else if (to >= adjacency_matrix_.size())
            {
                std::string error = "There is no vertex with index " + std::to_string(to);
                throw std::out_of_range(error);
            }
            if (next[from][to] == _INF)
                return {};

            std::vector<Vertex> path;
            Vertex cur_v = from;
            path.push_back(cur_v);

            while (cur_v != to)
            {
                cur_v = next[cur_v][to];
                path.push_back(cur_v);
            }

            return path;
        }

        bool CurrencyArbitrage(const AdjacencyMatrix &currency_rates)
        {
            size_t vertex_count = currency_rates.size();
            AdjacencyMatrix log_currency_rates(vertex_count, std::vector<Weight>(vertex_count));

            for (size_t i = 0; i < vertex_count; ++i)
            {
                for (size_t j = 0; j < vertex_count; ++j)
                {
                    if (currency_rates[i][j] > 0)
                    {
                        log_currency_rates[i][j] = -std::log(currency_rates[i][j]);
                    }
                    else
                    {
                        log_currency_rates[i][j] = _INF;
                    }
                }
            }
            
            for (size_t i = 0; i < vertex_count; ++i)
            {
                for (size_t u = 0; u < vertex_count; ++u)
                {
                    for (size_t v = 0; v < vertex_count; ++v)
                    {
                        if (log_currency_rates[u][v] > log_currency_rates[u][i] + log_currency_rates[i][v])
                        {
                            log_currency_rates[u][v] = log_currency_rates[u][i] + log_currency_rates[i][v];
                        }
                    }
                }
            }

            for (size_t i = 0; i < vertex_count; ++i)
            {
                if (log_currency_rates[i][i] < 0)
                {
                    return true;
                }
            }

            return false;
        }

    private:
        AdjacencyMatrix adjacency_matrix_;
    };

}
#endif // _GRAPH_H_