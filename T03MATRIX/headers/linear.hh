
#include "containers.hh"

#ifndef LINEAR_HH_INCL
#define LINEAR_HH_INCL

namespace linear
{

// Interface for mathematical vector with size fixed on obj creation.
template<class T>
struct IMathVector
{
    virtual ~IMathVector() {}

    virtual size_t size() noexcept;

    // Access to vector elements:
    virtual T &getComponent(size_t) noexcept;
    virtual const T &getComponent(size_t) const noexcept;

    // Operations should be linear:
    virtual IMathVector &operator-() &;
    virtual IMathVector &operator+=(const IMathVector&) &;
    virtual IMathVector &operator-=(const IMathVector&) &;
    virtual IMathVector &operator*=(const T&) &;
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
        for (auto it = l.begin(), end = l.end(); it != end; ++it)
            data_[i] = *it;
    }

    static std::unique_ptr<MathVector> create(size_t size)
    {
        return new MathVector(size);
    }

    static std::unique_ptr<MathVector> create(std::initializer_list<T> l)
    {
        return new MathVector(l);
    }

    size_t size() override noexcept
    {
        return size_;
    }

    T &getComponent(size_t id) override noexcept
    {
        return data_[id % size_];
    }
    const T &getComponent(size_t id) const override noexcept
    {
        return data_[id % size_];
    }

    MathVector &operator-() & override
    {
        for (size_t i = 0; i < size_; ++i)
            data_[i] = -data_[i];
    }

    MathVector &operator+=(const IMathVector<T> &sd) & override
    {
        if (typeid(sd) == typeid(const MathVector))
        {
            const MathVector &second = dynamic_cast<const MathVector>(sd);
            for (size_t i = 0; i < size_; ++i)
                data_[i] += second.data_[i];
            return *this;
        }

        for (size_t i = 0; i < size_; ++i)
            data_[i] += sd.getComponent(i);
        return *this;
    }
    MathVector &operator-=(const IMathVector<T> &sd) & override
    {
        if (typeid(sd) == typeid(const MathVector))
        {
            const MathVector &second = dynamic_cast<const MathVector>(sd);
            for (size_t i = 0; i < size_; ++i)
                data_[i] -= second.data_[i];
            return *this;
        }

        for (size_t i = 0; i < size_; ++i)
            data_[i] += sd.getComponent(i);
        return *this;
    }

    MathVector &operator*=(const T& num) override
    {
        for (size_t i = 0; i < size_; ++i)
            data_[i] *= num;
    }
};

template <class T> class IMatrix : public IMathVector
{
    using Row = IMathVector<T>;

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
    virtual ProxyRow operator[](size_t) noexcept;
    virtual const ProxyRow operator[](size_t) const noexcept;

    virtual xSize() const noexcept;
    virtual ySize() const noexcept;
};

template <class T> struct ISquareMatrix : public IMatrix
{
    virtual T det() const;
};

template <class T> class SquareMatrix final : public ISquareMatrix
{
    using IMatrix::Row;
    using IMatrix::ProxyRow;

    containers::Vector<std::unique_ptr<Row>> data_{};

    const size_t rowSize_;
};

} // namespace linear

#endif
