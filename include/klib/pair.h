#if !defined(__INCLUDE_KLIB_KPAIR_H)
#define __INCLUDE_KLIB_KPAIR_H

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

template <typename TF, typename TS>
struct Pair
{
    TF first;
    TS second;
    Pair(void) : first(0), second(0) {}
    Pair(TF f, TS s) : first(f), second(s) {}
    ~Pair() {}
};

#endif // __INCLUDE_KLIB_KPAIR_H
