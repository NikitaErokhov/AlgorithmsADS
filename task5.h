#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <stdexcept>
#include <string>
#include <iostream>
#include <stack>

namespace graph
{
    using Vertex = size_t;
    using Verticies = std::vector<Vertex>;

    class Graph
    {
    public:
        Graph() : graph_() {}
        Graph(size_t vertex_count) : graph_(vertex_count) {}
        Graph(const Graph &rhs) : graph_(rhs.graph_) {}
        Graph(Graph &&rhs) noexcept : graph_(std::move(rhs.graph_)) {}

        size_t size() const { return graph_.size(); }
        bool empty() const { return !size(); }

        void resize(size_t new_vertex_count)
        {
            graph_.clear();
            graph_.resize(new_vertex_count);
        }

        const std::vector<Vertex> &GetAdjacents(Vertex v) const
        {
            return graph_.at(v);
        }

        // std::vector<Vertex>& GetAdjacents(Vertex v) {
        //     return graph_.at(v);
        // }

        void AddEdge(Vertex from, Vertex to)
        {
            if (to >= graph_.size())
            {
                std::string error = "There_is no vertex with index " + std::to_string(to);
                throw std::logic_error(error);
            }
            graph_.at(from).push_back(to);
        }

        void GoDFS(Vertex v, std::vector<graph::Graph::Colors> &colors, const graph::Graph &gr)
        {
            switch (colors[v])
            {
            case (Colors::White):
                colors[v] = Colors::Gray;
                std::cout << 'Pass ' << v << std::endl;
                for (Vertex w : gr.GetAdjacents(v))
                {
                    GoDFS(w, colors, gr);
                }
                colors[v] = Colors::Black;
                break;
            case (Colors::Gray):
                std::cout << 'Has cycle at ' << v << std::endl;
                break;
            case (Colors::Black):
                std::cout << 'Has comleted vertex at ' << v << std::endl;
                break;
            }
        }

        void DFS(const graph::Graph &gr)
        {
            std::vector<graph::Graph::Colors> colors{gr.size(), Colors::White};
            for (Vertex v = 0; v < gr.size(); ++v)
            {
                if (colors[v] == Colors::White)
                {
                    GoDFS(v, colors, gr);
                }
            }
        }
        
        Graph InvertGraph() const
        {
            // Создаём пустой граф, который булем заполнять, кол-во вершин то же
            Graph inverted_graph_(size());

            // Перебираем каждую вершинку исходного
            for (Vertex v = 0; v < size(); ++v)
            {
                for (Vertex u : graph_[v])
                {
                    // Добавляем для тех же вершин обратное ребро
                    // было v->u стало v<-u
                    inverted_graph_.AddEdge(u, v);
                }
            }

            return inverted_graph_;
        }
        std::vector<std::vector<Vertex>> GetStrongComponents() const
        {
            if (empty())
            {
                return {};
            }

            // создаём список цветов, где все вершины белые, для DFS по исходному
            std::vector<graph::Graph::Colors> colors{size(), Colors::White};

            // За условную единицу времени берём начало нового DFS или прохождение по ребру
            // Вместо меток, заведём стэк для зранения вершин по "времени" выхода из рекурсии по убыванию
            std::stack<Vertex> vertex_order;

            // Первый DFS с заполнением стэка
            // Каждый раз будем брать первую непомеченную вершину
            for (Vertex v = 0; v < size(); ++v)
            {
                if (colors[v] == Colors::White)
                {
                    GetOrderDFS(v, colors, vertex_order);
                }
            }

            // Инвертируем граф
            Graph inverted_graph_ = InvertGraph();

            // Создаём vector цветов для DFS по инвертированному графу
            std::vector<graph::Graph::Colors> colors_inv{size(), Colors::White};

            // Создаём вектор для сбора компонент сильной связности
            std::vector<std::vector<Vertex>> strong_components;

            // Второй DFS для формирования каждой сильной компоненты
            while (!vertex_order.empty())
            {
                // Вытаскиваем вершины из стэка
                Vertex v = vertex_order.top();
                vertex_order.pop();

                // Новой компоненте соответствуют только раннее необработанные
                if (colors_inv[v] == Colors::White)
                {
                    // Создаём вектор для хранения вершин новой компоненты
                    // P.S. Вместо вектора можно создавать новый граф, но в данном случае считаю вектор избыточным
                    std::vector<Vertex> component;
                    GetComponentDFS(v, inverted_graph_, colors_inv, component);
                    strong_components.push_back(component);
                }
            }

            /* Время работы
            1) Оценка снизу 
                Инвертация графа за O(V + E)
                оба DFS за O(V + E), поскольку отличия минимальны от классического DFS
                => Оценка снизу даёт O(V + E)
            2) Оценка сверху
                Устремим E к V(V-1)/2
                получит O(V + (V^2 - V)/2) = ... = O(V^2)
                => Оценка сверху даёт O(V^2)
            */

            return strong_components;
        }

    private:
        enum class Colors
        {
            White,
            Gray,
            Black
        };

        std::vector<std::vector<Vertex>> graph_;

        void GetOrderDFS(Vertex v,
                         std::vector<graph::Graph::Colors> &colors,
                         std::stack<Vertex> &vertex_order) const
        {
            // Основное отличие от классического DFS - заполнение стэка

            // Вершина серая - её обработка началась
            colors[v] = Colors::Gray;

            for (Vertex u : GetAdjacents(v))
            {
                if (colors[u] == Colors::White)
                {
                    GetOrderDFS(u, colors, vertex_order);
                }
            }

            // Вершина добавится в стек только после отбработки DFS от всех её соседей
            vertex_order.push(v);
            // Вершина, с которой начался обход, в итоге на вершине стека - имеет наибольшее время выхода из рекурсии

            // Вершина черная - её обработка завершилась
            colors[v] = Colors::Black;
        }

        void GetComponentDFS(Vertex v,
                             const Graph &inverted_graph_,
                             std::vector<graph::Graph::Colors> &colors,
                             std::vector<Vertex> &component) const
        {

            // Основное отличие от классического DFS - заполнение стэка

            // Вершина серая - её обработка началась
            colors[v] = Colors::Gray;

            // Вершина, для которой пришёл вызов GetComponentDFS - часть текущей component
            component.push_back(v);

            for (Vertex u : inverted_graph_.GetAdjacents(v))
            {
                if (colors[u] == Colors::White)
                {
                    GetComponentDFS(u, inverted_graph_, colors, component);
                }
            }

            // Вершина черная - её обработка завершилась
            colors[v] = Colors::Black;
        }
    };

}
#endif // _GRAPH_H_