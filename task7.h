#include <vector>

template<class T, class OPERATION>
class SegmentTree
{
public:
    SegmentTree( const std::vector<T> &vec, OPERATION op, const T& fill_value ): operation(op)
    {
        size_t n0 = vec.size();
        n = 1;
        while (n < n0) {
            //n *= 2;
            n <<= 1;
        }
        segment_tree.resize(2*n-1, fill_value);

        for (size_t i = 0; i < n0; ++i) {
            segment_tree[GetNewInd(i)] = vec[i];
        }
        for (int i = n - 2; i >= 0; --i) {
            // 2*i+1 - левый ребенок, 2*i+2 - правый ребенок
            segment_tree[i] = operation(segment_tree[2*i+1], segment_tree[2*i+2]);
        }
    }
    T Request( size_t left, size_t right)
    {
        T ans{};
        size_t Left = GetNewInd(left);
        size_t Right = GetNewInd(right);
        while( Left < Right )
        {
            if( Left%2==0 )
            {
                ans = operation(ans, segment_tree[Left]);
                Left++;
            }
            if( Right%2==1 )
            {
                ans = operation(ans, segment_tree[Right]);
                Right--;
            }
            Left = Parent(Left);
            Right = Parent(Right);
        }
        ans = operation(ans, segment_tree[Left]);
        return ans;
    }
private:
    std::vector<T> segment_tree; // 2*2^k -1, n0<=2^k=n
    size_t n; // = 2^k
    OPERATION operation;
    size_t Parent( size_t i )
    {
        return (i-1)/ 2;
    }
    size_t GetNewInd( size_t i )
    {
        return n-1+i;
    }
};