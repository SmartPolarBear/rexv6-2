#if !defined(__INCLUDE_KLIB_ALLOCATOR_H)
#define __INCLUDE_KLIB_ALLOCATOR_H

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

namespace klib
{

template <typename T>
struct allocator
{
    void release(T *p)
    {
        delete[] p;
    }

    T *allocate(unsigned int size)
    {
        return new T[size];
    }
};

} // namespace klib

#endif // __INCLUDE_KLIB_ALLOCATOR_H
