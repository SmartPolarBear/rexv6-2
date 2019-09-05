#if !defined(__INCLUDE_KLIB_QUEUE_H)
#define __INCLUDE_KLIB_QUEUE_H

#include "klib/internal/allocator.h"

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

namespace klib
{

template <typename TElement, typename TCont, typename TAllocator = allocator<TElement>, typename TPred = less<TElement>>
class PriorityQueue
{
public:
    using value_type = TElement;
    using reference = TElement &;
    using const_reference = const TElement &;
    using alloc_type = TAllocator;
    using pred_type = TPred;
    using size_type = unsigned int;
    using container_type = TCont;

private:
    alloc_type allocator;
    container_type container;

public:
    PriorityQueue(void);
    PriorityQueue(const PriorityQueue &prq);
    ~PriorityQueue();

    void push(value_type element);
    void pop(void);
    value_type top(void);
    size_type size(void) const;
    [[nodiscard]] bool empty(void) const;
};
template <typename TElement, typename TCont, typename TAllocator, typename TPred>
PriorityQueue<TElement, TCont, TAllocator, TPred>::PriorityQueue(void)
{
    container.clear();
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
PriorityQueue<TElement, TCont, TAllocator, TPred>::PriorityQueue(const PriorityQueue &prq)
{
    container.clear();
    for (int i = 0; i < prq.container.size(); i++)
    {
        this.container.push_back(prq.container[i]);
    }
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
PriorityQueue<TElement, TCont, TAllocator, TPred>::~PriorityQueue(void)
{
    container.clear();
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
[[nodiscard]] bool PriorityQueue<TElement, TCont, TAllocator, TPred>::empty(void) const {
    return container.empty();
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
typename PriorityQueue<TElement, TCont, TAllocator, TPred>::size_type PriorityQueue<TElement, TCont, TAllocator, TPred>::size(void) const
{
    return container.size();
}

} // namespace klib

#endif // __INCLUDE_KLIB_QUEUE_H
