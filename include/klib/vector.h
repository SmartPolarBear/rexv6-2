#if !defined(__INCLUDE_KLIB_VECTOR_H)
#define __INCLUDE_KLIB_VECTOR_H

#include "klib/allocator.h"

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

namespace klib
{

template <typename TEle, typename TAlloc=allocator<TEle>>
class Vector
{

public:
    using value_type = TEle;
    using size_type = unsigned int;
    using reference = TEle &;
    using const_reference = const TEle &;
    using alloc_type = TAlloc;

    using iterator_type = TEle *;
    using const_iterator_type = const TEle *;

private:
    unsigned int get_capacity(unsigned int);
    unsigned int size_;
    unsigned int capacity_;

    TEle *data_;
    TAlloc alloc;

public:
    Vector(size_type = 0);
    Vector(const Vector &);
    ~Vector(void);

    size_type size();
    bool empty();
    size_type capacity();
    void reserve(size_type);
    void shrink_to_fit();
    void push_back(value_type);
    void pop_back();

    iterator_type begin();
    iterator_type end();
    const_iterator_type cbegin();
    const_iterator_type cend();
    iterator_type erase(const_iterator_type pos);
    iterator_type insert(const_iterator_type pos, value_type val);

    reference operator[](size_type);
};

template <typename TEle, typename TAlloc>
Vector<TEle, TAlloc>::Vector(unsigned int s)
    : size_(0), capacity_(0), data_(nullptr)
{
    size_ = s;
    if (s != 0)
    {
        capacity_ = get_capacity(s);
        data_ = alloc.allocate(capacity_); //new T[capacity_];
    }
    data_ = alloc.allocate(1); //new T[1];
}

template <typename TEle, typename TAlloc>
Vector<TEle, TAlloc>::Vector(const Vector &obj)
    : size_(0), capacity_(0), data_(nullptr)
{

    size_ = obj.size_;
    capacity_ = obj.capacity_;

    data_ = alloc.allocate(capacity_); //new T[capacity_];
    for (decltype(size_) i = 0u; i < size_; ++i)
    {
        data_[i] = obj.data_[i];
    }
}

template <typename TEle, typename TAlloc>
Vector<TEle, TAlloc>::~Vector()
{
    if (data_ != nullptr)
    {
        alloc.release(data_);
        // delete[] data_;
    }
}

template <typename TEle, typename TAlloc>
typename Vector<TEle, TAlloc>::size_type Vector<TEle, TAlloc>::size()
{
    return size_;
}

template <typename TEle, typename TAlloc>
bool Vector<TEle, TAlloc>::empty()
{
    return size_ == 0;
}

template <typename TEle, typename TAlloc>
typename Vector<TEle, TAlloc>::size_type Vector<TEle, TAlloc>::capacity()
{
    return capacity_;
}

template <typename TEle, typename TAlloc>
typename Vector<TEle, TAlloc>::size_type Vector<TEle, TAlloc>::get_capacity(typename Vector<TEle, TAlloc>::size_type n)
{
    if (n == 0)
        return 0;
    unsigned int mask = 1;
    mask <<= 31;

    while (!(mask & n))
        mask >>= 1;

    if (mask != n)
        mask <<= 1;

    return mask;
}

template <typename TEle, typename TAlloc>
void Vector<TEle, TAlloc>::shrink_to_fit()
{
    capacity_ = get_capacity(size_);
    TEle *temp = alloc.allocate(size_); //new T[size_];

    for (auto i = 0u; i < size_; ++i)
    {
        temp[i] = data_[i];
    }

    alloc.release(data_);
    // delete[] data_;

    data_ = alloc.allocate(capacity_); //new T[capacity_];

    for (decltype(size_) i = 0u; i < size_; ++i)
    {
        data_[i] = temp[i];
    }
}

template <typename TEle, typename TAlloc>
void Vector<TEle, TAlloc>::reserve(typename Vector<TEle, TAlloc>::size_type nsz)
{
    if (nsz >= capacity_)
    {
        capacity_ = get_capacity(nsz);
        TEle *temp = alloc.allocate(size_); //new T[size_];

        for (auto i = 0u; i < size_; ++i)
        {
            temp[i] = data_[i];
        }

        alloc.release(data_);
        // delete[] data_;

        data_ = alloc.allocate(capacity_); //new T[capacity_];

        for (decltype(size_) i = 0u; i < size_; ++i)
        {
            data_[i] = temp[i];
        }
    }
}

template <typename TEle, typename TAlloc>
void Vector<TEle, TAlloc>::push_back(TEle val)
{
    if (size_ + 1 >= capacity_)
    {
        reserve(size_ + 1);
    }
    size_++;
    data_[size_ - 1] = val;
}

template <typename TEle, typename TAlloc>
void Vector<TEle, TAlloc>::pop_back()
{
    if (size_ == 0)
    {
        return;
    }

    size_--;
}

template <typename TEle, typename TAlloc>
typename Vector<TEle, TAlloc>::reference Vector<TEle, TAlloc>::operator[](typename Vector<TEle, TAlloc>::size_type i)
{
    return data_[i];
}

template <typename TEle, typename TAlloc>
typename Vector<TEle, TAlloc>::iterator_type Vector<TEle, TAlloc>::begin()
{
    return data_;
}

template <typename TEle, typename TAlloc>
typename Vector<TEle, TAlloc>::iterator_type Vector<TEle, TAlloc>::end()
{
    return data_ + size_;
}

template <typename TEle, typename TAlloc>
typename Vector<TEle, TAlloc>::const_iterator_type Vector<TEle, TAlloc>::cbegin()
{
    return static_cast<const_iterator_type>(data_);
}

template <typename TEle, typename TAlloc>
typename Vector<TEle, TAlloc>::const_iterator_type Vector<TEle, TAlloc>::cend()
{
    return static_cast<const_iterator_type>(data_ + size_);
}

template <typename TEle, typename TAlloc>
typename Vector<TEle, TAlloc>::iterator_type Vector<TEle, TAlloc>::erase(typename Vector<TEle, TAlloc>::const_iterator_type pos)
{
    auto diff = pos - data_;
    if (diff < 0 || static_cast<size_type>(diff) > size_)
    {
        return nullptr; //error
    }

    size_type current{static_cast<size_type>(diff)};
    for (decltype(current) i = current; i < size_ - 1; i++)
    {
        data_[i] = data_[i + 1];
    }

    size_--;
    return static_cast<iterator_type>(data_ + current);
}

template <typename TEle, typename TAlloc>
typename Vector<TEle, TAlloc>::iterator_type Vector<TEle, TAlloc>::insert(typename Vector<TEle, TAlloc>::const_iterator_type pos, typename Vector<TEle, TAlloc>::value_type val)
{
    auto diff = pos - data_;
    if (diff < 0u || static_cast<size_type>(diff) > size_)
    {
        return nullptr; //error
    }

    size_type current{static_cast<size_type>(diff)};

    if (size_ + 1 >= capacity_)
    {
        reserve(size_ + 1);
    }

    for (decltype(size_) i = size_; i > current; i--)
    {
        data_[i] = data_[i - 1];
    }

    size_++;
    data_[current] = val;
    return static_cast<iterator_type>(data_ + current);
}

} // namespace klib

#endif // __INCLUDE_KLIB_VECTOR_H
