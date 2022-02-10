

#include <type_traits>
#include <stdexcept>

#include "containers.hh"

#ifndef LINEAR_HH_INCL
#define LINEAR_HH_INCL

namespace linear
{

// Interface for mathematical vector with fixed dimensionality
// and elements of arithmetic type T. Objects of this class should
// be linear over field of numbers of type T. 
template<class T>
struct IMathVector
{
    static_assert(std::is_arithmetic_v<T>);

    virtual ~IMathVector() {}

    virtual size_t dim() noexcept;

    bool dimsAreEqual(const IMathVector& second) noexcept
    {
        return dim() == second.dim();
    }

    // Access to vector components:
    virtual T &component(size_t);
    virtual const T &component(size_t) const;

    virtual IMathVector &operator-() &
    {
        for (size_t i = 0, end = dim(); i < end; ++i)
        {
            auto val = component(i);
            val = -val;
        }

        return *this;
    }

    virtual IMathVector &operator+=(const IMathVector& second) &
    {
        if (!dimsAreEqual(second))
            throw std::out_of_range("Tried to use operator+= for objects of type linear::IMathVector with different dimensionality");
        for (size_t i = 0, end = dim(); i < end; ++i)
            component(i) += second.component(i);

        return *this;
    }
    virtual IMathVector &operator-=(const IMathVector& second) &
    {
        if (!dimsAreEqual(second))
            throw std::out_of_range("Tried to use operator-= for objects of type linear::IMathVector with different dimensionality");
        for (size_t i = 0, end = dim(); i < end; ++i)
            component(i) -= second.component(i);

        return *this;
    }

    virtual IMathVector &operator*=(const T& num) &
    {
        for (size_t i = 0, end = dim(); i < end; ++i)
            component(i) *= num;
    }
    virtual IMathVector &operator/=(const T& num) &
    {
        return (*this) *= (1. / num);
    }
};

// Common realization of math vector
template <class T> class MathVector final : public IMathVector<T>
{
    containers::Vector<T> data_;
    const size_t size_;

  public:
    MathVector(size_t size) : data_(size), size_(size) {}

    MathVector(std::initializer_list<T> l) : data_(l.size()), size_(l.size())
    {
        int i = 0;
        for (auto it = l.begin(), end = l.end(); it != end; ++it, ++i)
            data_[i] = *it;
    }

    size_t dim() noexcept override
    {
        return size_;
    }

    T &component(size_t id) override
    {
        if (id >= size_)
            throw std::out_of_range("Tried to access out of range component of linear::MathVector");

        return data_[id % size_];
    }
    const T &component(size_t id) const noexcept override
    {
        if (id >= size_)
            throw std::out_of_range("Tried to access out of range component of linear::MathVector");

        return data_[id % size_];
    }
};

template <class T> struct IMatrix : public IMathVector<T>
{
    using Row = IMathVector<T>;

  protected:
    struct ProxyRow
    {
        Row &row_;

        ProxyRow(Row &row) : row_{row}
        {
        }

        T &operator[](size_t id)
        {
            return row_.getComponent(id);
        }

        const T &operator[](size_t id) const
        {
            return row_.getComponent(id);
        }
    };

  public:
    virtual ProxyRow operator[](size_t);
    virtual const ProxyRow operator[](size_t) const;

    virtual size_t rowNum() const noexcept;
    virtual size_t colNum() const noexcept;
};

template <class T> struct ISquareMatrix : public IMatrix<T>
{
    virtual T det() const;
};

template <class T> class SquareMatrix final : public ISquareMatrix<T>
{
    using IMatrix<T>::Row;
    using IMatrix<T>::ProxyRow;

    containers::Vector<std::unique_ptr<typename IMatrix<T>::Row>> data_{};

    const size_t size_;

  public:
    SquareMatrix(size_t size) : data_(size, std::unique_ptr<typename IMatrix<T>::Row>(nullptr)), size_(size)
    {
        for (size_t i = 0; i < size; ++i)
            data_[i] = new MathVector<T>(size_);
    }

    size_t dim() noexcept override
    {
        return size_ * size_;
    }

    IMatrix<T>::ProxyRow operator[](size_t id) override
    {
        if (id >= size_)
            throw std::out_of_range("Tried to access out of range component of linear::SquareMatrix");
        return *(data_[id]);
    }
    const IMatrix<T>::ProxyRow operator[](size_t id) const override
    {
        if (id >= size_)
            throw std::out_of_range("Tried to access out of range component of linear::SquareMatrix");
        return *(data_[id]);
    }

    T &component(size_t id) override
    {
        if (id >= dim())
            throw std::out_of_range("Tried to access out of range component of linear::SquareMatrix");

        return (*this)[id / size_][id % size_];
    }
    const T &component(size_t id) const override
    {
        if (id >= dim())
            throw std::out_of_range("Tried to access out of range component of linear::SquareMatrix");

        return (*this)[id / size_][id % size_];
    }

    size_t rowNum() const noexcept override
    {
        return size_;
    }
    size_t colNum() const noexcept override
    {
        return size_;
    }

    T det() const override
    {
        return T{};
    }
};

} // namespace linear

#endif
