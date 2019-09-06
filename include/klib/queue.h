#if !defined(__INCLUDE_KLIB_QUEUE_H)
#define __INCLUDE_KLIB_QUEUE_H

#include "klib/internal/allocator.h"
#include "klib/internal/comp.h"

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

    TElement placeholder0;
    size_type nelement;

public:
    PriorityQueue(void);
    PriorityQueue(const PriorityQueue &prq);
    ~PriorityQueue();

    void push(value_type element);
    void pop(void);
    const_reference top(void) const;
    size_type size(void) const;
    [[nodiscard]] bool empty(void) const;
};
template <typename TElement, typename TCont, typename TAllocator, typename TPred>
PriorityQueue<TElement, TCont, TAllocator, TPred>::PriorityQueue(void)
{
    container.clear();
    container.push_back(placeholder0);
    nelement = 0;
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
PriorityQueue<TElement, TCont, TAllocator, TPred>::PriorityQueue(const PriorityQueue &prq)
{
    container.clear();
    for (int i = 0; i < prq.container.size(); i++)
    {
        this.container.push_back(prq.container[i]);
    }
    nelement = this.container.nelement;
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
PriorityQueue<TElement, TCont, TAllocator, TPred>::~PriorityQueue(void)
{
    container.clear();
    nelement = 0;
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
[[nodiscard]] bool PriorityQueue<TElement, TCont, TAllocator, TPred>::empty(void) const {
    return container.empty();
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
typename PriorityQueue<TElement, TCont, TAllocator, TPred>::size_type PriorityQueue<TElement, TCont, TAllocator, TPred>::size(void) const
{
    return nelement;
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
typename PriorityQueue<TElement, TCont, TAllocator, TPred>::const_reference PriorityQueue<TElement, TCont, TAllocator, TPred>::top(void) const
{
    return container[1];
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
void PriorityQueue<TElement, TCont, TAllocator, TPred>::pop(void)
{
}

template <typename TElement, typename TCont, typename TAllocator, typename TPred>
void PriorityQueue<TElement, TCont, TAllocator, TPred>::push(value_type element)
{
    if (container.empty())
    {
        container.push_back(placeholder0); //the container must have at least one element
    }

    container.push_back(element);
    nelement++;
    
    if (nelement > 1)
    {
        size_type p = nelement;
        while (p > 1)
        {
            if (container[p] > container[p / 2])
            {
                swap(container[p], container[p / 2]);
                p /= 2;
            }
            else
            {
                break;
            }
        }
    }
}

} // namespace klib

#endif // __INCLUDE_KLIB_QUEUE_H
