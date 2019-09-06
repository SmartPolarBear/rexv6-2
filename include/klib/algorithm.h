#if !defined(__INCLUDE_KLIB_ALGORITHM_H)
#define __INCLUDE_KLIB_ALGORITHM_H

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

#include "klib/move.h"

template <typename T>
void swap(T &t1, T &t2)
{
    T temp = std::move(t1);
    t1 = std::move(t2);
    t2 = std::move(temp);
}

#endif // __INCLUDE_KLIB_ALGORITHM_H
