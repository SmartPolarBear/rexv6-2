#if !defined(__INCLUDE_KLIB_PriorityQueue_H)
#define __INCLUDE_KLIB_PriorityQueue_H

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

template<typename TElement,typename TAllocator,typename TPred>
class PriorityQueue
{
private:
    using value_type = TElement;
    using reference = TElement &;
    using const_reference = const TElement &;
    using alloc_type = TAllocator;
    using pred_type = TPred;
    using size_type = unsigned int;

    alloc_type allocator;
public:
    PriorityQueue(void);
    ~PriorityQueue();

    void push(element_type element);
    void pop(void);
    element_type top(void);
    size_type size(void);
};

#endif // __INCLUDE_KLIB_PriorityQueue_H
