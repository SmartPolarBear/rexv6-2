#if !defined(__INCLUDE_KLIB_VECTOR_H)
#define __INCLUDE_KLIB_VECTOR_H

#if !defined(__cplusplus)
#error ONLY FOR C++
#endif

namespace klib
{

template <typename TEle, typename TAlloc>
class Vector
{

private:
    using value_type = TEle;
    using size_type = unsigned int;
    using reference = TEle &;
    using const_reference = const TEle &;
    using alloc_type = TAlloc;

    using iterator_type = TEle *;
    using const_iterator_type = const TEle *;

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
    size_type capacity();
    void push_back(value_type);
    void pop_back();

    reference operator[](size_type);
};

template <typename TEle, typename TAlloc>
Vector<TEle, TAlloc>::Vector(unsigned int s)
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
{

    size_ = obj.size_;
    capacity_ = obj.capacity_;

    data_ = alloc.allocate(capacity_); //new T[capacity_];
    for (int i = 0; i < size_; ++i)
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
unsigned int Vector<TEle, TAlloc>::size()
{
    return size_;
}
template <typename TEle, typename TAlloc>
unsigned int Vector<TEle, TAlloc>::capacity()
{
    return capacity_;
}

template <typename TEle, typename TAlloc>
unsigned int Vector<TEle, TAlloc>::get_capacity(unsigned int n)
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
void Vector<TEle, TAlloc>::push_back(TEle val)
{
    if (size_ + 1 > capacity_)
    {
        capacity_ = get_capacity(size_ + 1);
        TEle *temp = alloc.allocate(size_); //new T[size_];

        for (int i = 0; i < size_; ++i)
        {
            temp[i] = data_[i];
        }

        alloc.release(data_);
        // delete[] data_;

        data_ = alloc.allocate(capacity_); //new T[capacity_];

        for (int i = 0; i < size_; ++i)
        {
            data_[i] = temp[i];
        }
    }
    ++size_;
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
TEle &Vector<TEle, TAlloc>::operator[](unsigned int i)
{
    return data_[i];
}

} // namespace klib

#endif // __INCLUDE_KLIB_VECTOR_H
