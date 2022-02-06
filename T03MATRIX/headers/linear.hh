
#include <cstdlib>
#include <utility>

#ifndef LINEAR_HH_INCL
#define LINEAR_HH_INCL

namespace linear
{

// Exceptions safe dynamical allocated block storage.
template <class T> class VectorStorage
{
  protected:
    size_t initialized_ = 0;
    T *data_ = nullptr;

    VectorStorage(size_t size) : data_{static_cast<T *>(::operator new(sizeof(T) * size))}
    {
    }

    VectorStorage(VectorStorage &&toMove) noexcept
    {
        std::swap(initialized_, toMove.initialized_);
        std::swap(data_, toMove.data_);
    }

    VectorStorage &operator=(VectorStorage &&toMove) noexcept
    {
        std::swap(initialized_, toMove.initialized_);
        std::swap(data_, toMove.data_);
        return *this;
    }

    VectorStorage(const VectorStorage &) = delete;
    VectorStorage &operator=(const VectorStorage &) = delete;

    ~VectorStorage()
    {
        for (size_t i = 0; i < initialized_; ++i)
            (data_ + i)->~T();
        ::operator delete(data_);
    }
};

// Dynamical array with elements access.
template <class T> class Vector final : private VectorStorage<T>
{
    using VectorStorage<T>::initialized_;
    using VectorStorage<T>::data_;
    using VectorStorage<T>::VectorStorage;

    size_t size_ = 0;

  public:
    Vector(Vector &&) = default;
    Vector &operator=(Vector &&) = default;

    Vector(size_t size, const T &initVal = T{}) : VectorStorage<T>{size}, size_(size)
    {
        for (; initialized_ < size_; ++initialized_)
            new (data_ + initialized_) T{initVal};
    }

    Vector(const Vector &toCopy) : VectorStorage<T>{toCopy.size_}
    {
        for (; initialized_ < size_; ++initialized_)
            new (data_ + initialized_) T{toCopy.data_[initialized_]};
    }

    Vector &operator=(const Vector &toCopy)
    {
        if (&toCopy == this)
            return *this;

        Vector tmp{toCopy};
        return *this = std::move(tmp);
    }

    ~Vector() = default;

    T &operator[](size_t id) noexcept
    {
        return data_[id];
    }

    const T &operator[](size_t id) const noexcept
    {
        return data_[id];
    }
};

// Math vector with linear properties.
template <size_t SIZE, class T> class MathVector final
{
    Vector<T> data_{SIZE};

  public:
    T &operator[](size_t id)
    {
        return data_[id];
    }
    const T &operator[](size_t id) const
    {
        return data_[id];
    }

    MathVector &operator-() &
    {
        for (size_t i = 0; i < SIZE; ++i)
            data_[i] = -data_[i];
    }
    MathVector operator-() const &
    {
        MathVector copy{*this};
        -copy;
        return copy;
    }

    MathVector &operator+=(const MathVector &sd) &
    {
        for (size_t i = 0; i < SIZE; ++i)
            data_[i] += sd.data_[i];
    }
    MathVector &operator-=(const MathVector &sd) &
    {
        for (size_t i = 0; i < SIZE; ++i)
            data_[i] -= sd.data_[i];
    }
};

template <size_t SIZE, class T, class Convertable>
MathVector<SIZE, T> operator+(const MathVector<SIZE, T> &ft, const Convertable &sd)
{
    MathVector<SIZE, T> copy{ft};

    return copy += sd;
}
template <size_t SIZE, class T, class Convertable>
MathVector<SIZE, T> operator+(const Convertable &ft, const MathVector<SIZE, T> &sd)
{
    return sd + ft;
}

template <size_t SIZE, class T, class Convertable>
MathVector<SIZE, T> operator-(const MathVector<SIZE, T> &ft, const Convertable &sd)
{
    MathVector<SIZE, T> copy{sd};
    return copy -= ft;
}
template <size_t SIZE, class T, class Convertable>
MathVector<SIZE, T> operator-(const Convertable &ft, const MathVector<SIZE, T> &sd)
{
    return sd - ft;
}

template <size_t SIZE, class T> class SquareMatrix
{
    using row = MathVector<SIZE, T>;

    MathVector<SIZE, row> data_{};

    struct ProxyRow
    {
        row &row_;

        ProxyRow(row &row) : row_{row}
        {
        }

        T &operator[](size_t id)
        {
            return row_[id];
        }

        const T &operator[](size_t id) const
        {
            return row_[id];
        }
    };

  public:
    ProxyRow operator[](size_t id)
    {
        return ProxyRow{data_[id]};
    }
    const ProxyRow operator[](size_t id) const
    {
        return ProxyRow{data_[id]};
    }
};

} // namespace linear

#endif
