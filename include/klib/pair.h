#if !defined(__INCLUDE_KLIB_KPAIR_H)
#define __INCLUDE_KLIB_KPAIR_H

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

namespace klib
{

template <typename TF, typename TS>
struct Pair
{
    TF first;
    TS second;
    Pair(void) : first(0), second(0) {}
    Pair(TF f, TS s) : first(f), second(s) {}
    ~Pair() {}
};
} // namespace klib

#endif // __INCLUDE_KLIB_KPAIR_H
