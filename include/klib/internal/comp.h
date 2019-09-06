#if !defined(__INCLUDE_KLIB_INTERNAL_COMP_H)
#define __INCLUDE_KLIB_INTERNAL_COMP_H

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

namespace klib
{
template <typename T>
struct less
{
    constexpr bool operator()(const T &lhs, const T &rhs) const
    {
        return lhs < rhs;
    }
};

template <typename T>
struct greater
{
    constexpr bool operator()(const T &lhs, const T &rhs) const
    {
        return lhs > rhs;
    }
};
} // namespace klib

#endif // __INCLUDE_KLIB_INTERNAL_COMP_H
