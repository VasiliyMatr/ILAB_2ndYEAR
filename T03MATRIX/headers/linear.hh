
#include <type_traits>
#include <stdexcept>
#include <iostream>

#ifndef LINEAR_HH_INCL
#define LINEAR_HH_INCL

#include "containers.hh"

namespace linear
{

// Exceptions for linear namespace classes
struct IMV_dim_misalign final : public std::invalid_argument
{
    static constexpr const char* DEF_WHAT_ =
    "Tried to perform operation on IMathVector objects with different dimensionality";

    IMV_dim_misalign(const char* whatArg = DEF_WHAT_) : invalid_argument{whatArg} {}
};

struct IMV_out_of_range final : public std::out_of_range
{
    static constexpr const char* DEF_WHAT_ =
    "Tried to access IMathVector component by out of range id";

    IMV_out_of_range(const char* whatArg = DEF_WHAT_) : out_of_range{whatArg} {}
};

// Interface for mathematical vector with fixed dimensionality.
// Stores elements of type T. Objects of this class should be linear
// over field of numbers of type N.
template<class T, class N = T>
struct IMathVector
{
    static_assert(std::is_arithmetic_v<T>);

    virtual ~IMathVector() {}

    virtual size_t dim() const noexcept = 0;

    bool dimsAreEqual(const IMathVector& second) noexcept
    {
        return dim() == second.dim();
    }

    // Access to vector components:
    virtual T &component(size_t) = 0;
    virtual const T &component(size_t) const = 0;

    virtual IMathVector &operator-() &
    {
        for (size_t i = 0, end = dim(); i < end; ++i)
        {
            T val = component(i);
            val = -val;
        }

        return *this;
    }

    virtual IMathVector &operator+=(const IMathVector& second) &
    {
        if (!dimsAreEqual(second))
            throw IMV_dim_misalign();
        for (size_t i = 0, end = dim(); i < end; ++i)
            component(i) += second.component(i);

        return *this;
    }
    virtual IMathVector &operator-=(const IMathVector& second) &
    {
        if (!dimsAreEqual(second))
            throw IMV_dim_misalign();
        for (size_t i = 0, end = dim(); i < end; ++i)
            component(i) -= second.component(i);

        return *this;
    }

    virtual IMathVector &operator*=(const N& num) &
    {
        for (size_t i = 0, end = dim(); i < end; ++i)
            component(i) *= num;

        return *this;
    }
    virtual IMathVector &operator/=(const N& num) &
    {
        for (size_t i = 0, end = dim(); i < end; ++i)
            component(i) /= num;

        return *this;
    }
};

// Common implementation of IMathVector
template <class T, class N = T> class MathVector final : public IMathVector<T, N>
{
    containers::Vector<T> data_;

  public:
    MathVector() = default;

    MathVector(size_t size) : data_(size)
    {
    }
    MathVector(std::initializer_list<T> l) : data_(l)
    {
    }

    size_t dim() const noexcept override
    {
        return data_.size();
    }

    T &component(size_t id) override
    {
        if (id >= data_.size())
            throw IMV_out_of_range();

        return data_[id];
    }
    const T &component(size_t id) const override
    {
        if (id >= data_.size())
            throw IMV_out_of_range();

        return data_[id];
    }

    // Non virtual fast data access:
    T &operator[](size_t id)
    {
        return data_[id];
    }
    const T &operator[](size_t id) const
    {
        return data_[id];
    }
};

// Matrix is a vector of size rowNum x colNum
template <class T> struct IMatrix : public IMathVector<T>
{
    virtual size_t rowNum() const noexcept = 0;
    virtual size_t colNum() const noexcept = 0;
};

template <class T> struct ISquareMatrix : public IMatrix<T>
{
    virtual T det() const = 0;
};

// Common implementation of ISquareMatrix
template <class T> class SquareMatrix final : public ISquareMatrix<T>
{
    using Row = MathVector<T>;

    containers::Vector<Row> data_;

    const size_t size_;

  public:
    SquareMatrix(size_t size) : data_(size, Row(size)), size_(size)
    {
    }

    size_t dim() const noexcept override
    {
        return size_ * size_;
    }

    T &component(size_t id) override
    {
        if (id >= dim())
            throw IMV_out_of_range();

        return data_[id / size_][id % size_];
    }
    const T &component(size_t id) const override
    {
        if (id >= dim())
            throw IMV_out_of_range();

        return data_[id / size_][id % size_];
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

  private:
    // Non virtual fast data access:
    struct ProxyRow
    {
        Row &row_;
        ProxyRow(Row &row) : row_{row}
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
        return data_[id];
    }
    const ProxyRow operator[](size_t id) const
    {
        return data_[id];
    }
};

} // namespace linear

#endif
