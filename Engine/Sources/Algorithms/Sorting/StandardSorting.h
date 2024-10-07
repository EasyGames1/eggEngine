#ifndef ENGINE_SOURCES_ALGORITHMS_SORTING_FILE_STANDARD_SORT_H
#define ENGINE_SOURCES_ALGORITHMS_SORTING_FILE_STANDARD_SORT_H

#include <algorithm>
#include <functional>

namespace egg::Algorithms::Sorting
{
    struct StandardSorting
    {
        template <typename IteratorType, typename CompareType = std::less<>, typename... Args>
        void operator()(IteratorType First, IteratorType Last, CompareType Compare = CompareType {}, Args&&... Arguments)
        {
            std::sort(std::forward<Args>(Arguments)..., std::move(First), std::move(Last), std::move(Compare));
        }
    };
}

#endif // ENGINE_SOURCES_ALGORITHMS_SORTING_FILE_STANDARD_SORT_H
