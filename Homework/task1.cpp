#include <iostream>
#include <vector>

int ModificatedBinarySearch(const std::vector<int> &vec, int target)
{
    int left = 0, right = vec.size() - 1;
    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        if (vec[mid] == target)
        {
            return mid;
        }
        if (vec[left] <= vec[mid])
        {
            if (vec[left] <= target && target < vec[mid])
            {
                right = mid - 1;
            }
            else
            {
                left = mid + 1;
            }
        }
        else
        {
            if (vec[mid] < target && target <= vec[right])
            {
                left = mid + 1;
            }
            else
            {
                right = mid - 1;
            }
        }
    }

    return -1;
}