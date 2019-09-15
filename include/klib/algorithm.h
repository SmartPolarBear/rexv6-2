#if !defined(__INCLUDE_KLIB_ALGORITHM_H)
#define __INCLUDE_KLIB_ALGORITHM_H

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

#include "klib/move.h"

namespace klib
{

template <typename T>
void swap(T &t1, T &t2)
{
    T temp = move(t1);
    t1 = move(t2);
    t2 = move(temp);
}
} // namespace klib

#endif // __INCLUDE_KLIB_ALGORITHM_H
