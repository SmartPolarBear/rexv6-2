#if !defined(__INCLUDE_KLIB_QUEUE_H)
#define __INCLUDE_KLIB_QUEUE_H

#include "klib/internal/allocator.h"
#include "klib/internal/comp.h"
#include "klib/algorithm.h"
#include "klib/vector.h"

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

namespace klib
{

template <typename TElement, typename TCont = Vector<TElement>, typename TPred = less<TElement>, typename TAllocator = allocator<TElement>>
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
    TPred comp;

    TElement placeholder0;
    size_type nelement;

public:
    PriorityQueue(void);
    PriorityQueue(const PriorityQueue &prq);
    ~PriorityQueue();

    void push(value_type element);
    void pop(void);
    const_reference top(void);
    size_type size(void) const;
    [[nodiscard]] bool empty(void) const;
};
template <typename TElement, typename TCont, typename TPred, typename TAllocator>
PriorityQueue<TElement, TCont, TPred, TAllocator>::PriorityQueue(void)
{
    container.clear();
    container.push_back(placeholder0);
    nelement = 0;
}

template <typename TElement, typename TCont, typename TPred, typename TAllocator>
PriorityQueue<TElement, TCont, TPred, TAllocator>::PriorityQueue(const PriorityQueue &_prq)
{
    container.clear();
    PriorityQueue &prq = (PriorityQueue &)_prq;
    for (size_type i = 0; i < prq.container.size(); i++)
    {
        this->container.push_back(prq.container[i]);
    }
    nelement = prq.nelement;
}

template <typename TElement, typename TCont, typename TPred, typename TAllocator>
PriorityQueue<TElement, TCont, TPred, TAllocator>::~PriorityQueue(void)
{
    container.clear();
    nelement = 0;
}

template <typename TElement, typename TCont, typename TPred, typename TAllocator>
[[nodiscard]] bool PriorityQueue<TElement, TCont, TPred, TAllocator>::empty(void) const {
    return nelement == 0;
}

template <typename TElement, typename TCont, typename TPred, typename TAllocator>
typename PriorityQueue<TElement, TCont, TPred, TAllocator>::size_type PriorityQueue<TElement, TCont, TPred, TAllocator>::size(void) const
{
    return nelement;
}

template <typename TElement, typename TCont, typename TPred, typename TAllocator>
typename PriorityQueue<TElement, TCont, TPred, TAllocator>::const_reference PriorityQueue<TElement, TCont, TPred, TAllocator>::top(void)
{
    return container[1];
}

template <typename TElement, typename TCont, typename TPred, typename TAllocator>
void PriorityQueue<TElement, TCont, TPred, TAllocator>::pop(void)
{
    if (nelement == 0)
    {
        return;
    }

    swap(container[1], container[nelement]);
    container.pop_back();
    nelement--;

    size_type p = 1, s = 2, n = nelement;
    while (s <= n)
    {
        // container[s] < container[s + 1]
        if (s < n && comp(container[s], container[s + 1]))
            s++;
        // container[s] > container[p]
        if (!comp(container[s], container[p]))
        {
            swap(container[s], container[p]);
            p = s;
            s = p * 2;
        }
        else
        {
            break;
        }
    }
}

template <typename TElement, typename TCont, typename TPred, typename TAllocator>
void PriorityQueue<TElement, TCont, TPred, TAllocator>::push(value_type element)
{
    if (container.empty())
    {
        container.push_back(placeholder0); //the container must have at least one element
    }

    container.push_back(element);
    nelement++;

    size_type p = nelement;
    while (p > 1)
    {
        // container[p] > container[p / 2]
        if (!comp(container[p], container[p / 2]))
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

} // namespace klib

#endif // __INCLUDE_KLIB_QUEUE_H
