#if !defined(__INCLUDE_KLIB_ALGORITHM_H)
#define __INCLUDE_KLIB_ALGORITHM_H

#include <type_traits>

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

namespace klib
{

template <typename _Tp>
constexpr typename std::remove_reference_t<_Tp> &&
move(_Tp &&__t) noexcept
{
    return static_cast<typename std::remove_reference_t<_Tp> &&>(__t);
}

} // namespace klib

#endif // __INCLUDE_KLIB_ALGORITHM_H
