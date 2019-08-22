#if !defined(__INCLUDE_KLIB_KPAIR_H)
#define __INCLUDE_KLIB_KPAIR_H

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

template <typename TF, typename TS>
struct kpair
{
    TF first;
    TS second;
    kpair(void) : first(0), second(0) {}
    kpair(TF f, TS s) : first(f), second(s) {}
    ~kpair() {}
};

#endif // __INCLUDE_KLIB_KPAIR_H
