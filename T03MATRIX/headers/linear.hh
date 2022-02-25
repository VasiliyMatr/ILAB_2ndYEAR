
#include <type_traits>
#include <stdexcept>
#include <iostream>
#include <unordered_map>
#include <cmath>

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
    virtual ~IMathVector() {}

    virtual size_t dim() const noexcept = 0;

    bool dimsAreEqual(const IMathVector& second) const noexcept
    {
        return dim() == second.dim();
    }

    // Access to vector components:
    virtual T &at(size_t) = 0;
    virtual const T &at(size_t) const = 0;

  public:
    virtual IMathVector &operator-() &
    {
        for (size_t i = 0, end = dim(); i < end; ++i)
        {
            T &val = at(i);
            val = -val;
        }

        return *this;
    }

    virtual IMathVector &operator+=(const IMathVector& second) &
    {
        if (!dimsAreEqual(second))
            throw IMV_dim_misalign();
        for (size_t i = 0, end = dim(); i < end; ++i)
            at(i) += second.at(i);

        return *this;
    }
    virtual IMathVector &operator-=(const IMathVector& second) &
    {
        if (!dimsAreEqual(second))
            throw IMV_dim_misalign();
        for (size_t i = 0, end = dim(); i < end; ++i)
            at(i) -= second.at(i);

        return *this;
    }

    virtual IMathVector &operator*=(const N& num) &
    {
        for (size_t i = 0, end = dim(); i < end; ++i)
            at(i) *= num;

        return *this;
    }
    virtual IMathVector &operator/=(const N& num) &
    {
        for (size_t i = 0, end = dim(); i < end; ++i)
            at(i) /= num;

        return *this;
    }

    virtual void MAdd(const IMathVector &toMul, N num) &
    {
        if (!dimsAreEqual(toMul))
            throw IMV_dim_misalign();

        for (size_t i = 0, end = dim(); i < end; ++i)
            at(i) += toMul.at(i) * num;
    }
};

// Common implementation of IMathVector
template <class T, class N = T> class MathVector final : public IMathVector<T, N>
{
    containers::Vector<T> data_;

  public:
    MathVector() = default;
    MathVector(const MathVector&) = default;
    MathVector(MathVector&&) = default;
    MathVector& operator=(const MathVector&) = default;
    MathVector& operator=(MathVector&&) = default;

    MathVector(size_t size) : data_(size)
    {
    }
    MathVector(std::initializer_list<T> l) : data_(l)
    {
    }
    template<class CopyT, class CopyN = CopyT>
    MathVector<CopyT, CopyN> copy() const
    {
        MathVector<CopyT, CopyN> toRet(data_.size());
        for (size_t i = 0, end = data_.size(); i < end; ++i)
            toRet.at(i) = at(i);
        return toRet;
    }

    size_t dim() const noexcept override
    {
        return data_.size();
    }

    T &at(size_t id) override
    {
        if (id >= data_.size())
            throw IMV_out_of_range();

        return data_[id];
    }
    const T &at(size_t id) const override
    {
        if (id >= data_.size())
            throw IMV_out_of_range();

        return data_[id];
    }

    static void swap(MathVector &ft, MathVector &sd) noexcept
    {
        containers::Vector<T>::swap(ft.data_, sd.data_);
    }
};

// Matrix is a vector of size lineNum x colNum
template <class T> struct IMatrix : public IMathVector<T>
{
    virtual size_t lineNum() const noexcept = 0;
    virtual size_t colNum() const noexcept = 0;
};

template <class T, bool ENABLE_UNSIGNED_ = false> struct ISquareMatrix : public IMatrix<T>
{
    static_assert(ENABLE_UNSIGNED_ || !std::is_unsigned_v<T>,
        "SquareMatrix can't store unsigned values by default due to determinant calc issues." "\n"
        "You can enable unsigned values by changing template bool parameter value, "
        "but det function could return wrong values." "\n"
    );

  protected:
    virtual T constDetImpl() const = 0;
    virtual T detImpl() = 0;

  public:
    virtual T trace() const
    {
        T trace = 1;
        for (size_t i = 0, size = this->lineNum(); i < size; ++i)
            trace *= this->at(i * size + i);
        return trace;
    }

    T det() const
    {
        return constDetImpl();
    }
    T det(bool canModify = false)
    {
        return canModify ? detImpl() : constDetImpl();
    }
};

// Common implementation of ISquareMatrix
template <class T> class SquareMatrix final : public ISquareMatrix<T>
{
    using Line = MathVector<T>;

    const size_t size_;

    containers::Vector<Line> data_;

  public:
    SquareMatrix(size_t size) : size_{size}, data_(size_, Line(size_))
    {
    }
    SquareMatrix(std::initializer_list<T> l) :
        size_(std::sqrt(l.size())), data_(size_, Line(size_))
    {
        size_t i = 0;
        for (auto it = l.begin(), end = l.end(); it != end; ++it, ++i)
            at(i) = *it;
    }
    template<class CopyT>
    SquareMatrix<CopyT> copy() const
    {
        SquareMatrix<CopyT> copy (size_);
        for (size_t i = 0, end = size_ * size_; i < end; ++i)
            copy.at(i) = at(i);
        return copy;
    }

    size_t dim() const noexcept override
    {
        return size_ * size_;
    }

    T &at(size_t id) override
    {
        if (id >= dim())
            throw IMV_out_of_range();

        return data_[id / size_].at(id % size_);
    }
    const T &at(size_t id) const override
    {
        if (id >= dim())
            throw IMV_out_of_range();

        return data_[id / size_].at(id % size_);
    }

    size_t lineNum() const noexcept override
    {
        return size_;
    }
    size_t colNum() const noexcept override
    {
        return size_;
    }

  private:
    T detImpl() override;
    T constDetImpl() const override;

    // Lazy Gauss algo for detImpl method.
    // Works properly only for floating point numbers.
    void lazyGauss();

    template<class NumT = T, std::enable_if_t<std::is_arithmetic_v<NumT>, int> = 0>
    bool isZero(T &value)
    {
        static const double FP_CMP_PREC_ = 1E-6;
        return std::abs(value) < FP_CMP_PREC_;
    }

    // lazyGauss subfunc. Seeks for first line with not zero leading value.
    size_t seekNextLine(size_t lineIndex)
    {
        for (size_t i = lineIndex; i < size_; ++i)
            if (!isZero(data_[i].at(lineIndex)))
                return i;
        return size_;
    }

    // Handy data access stuff.
    struct ProxyLine
    {
        Line &line_;
        ProxyLine(Line &line) : line_{line}
        {
        }

        T &operator[](size_t id)
        {
            return line_.at(id);
        }
        const T &operator[](size_t id) const
        {
            return line_.at(id);
        }
    };

  public:
    ProxyLine operator[](size_t id)
    {
        return data_[id];
    }
    const ProxyLine operator[](size_t id) const
    {
        return data_[id];
    }

    void swapCols(size_t ftIndx, size_t sdIndx) noexcept
    {
        MathVector<T>::swap(data_[ftIndx], data_[sdIndx]);
    }

    void MAddCols(size_t ftIndx, size_t sdIndx, T num) noexcept
    {
        data_[ftIndx].MAdd(data_[sdIndx], num);
    }
};

using fp_t = double;

template<class T>
T SquareMatrix<T>::constDetImpl() const
{
    SquareMatrix<fp_t> mat = copy<fp_t>();
    fp_t result = mat.det(true); // canModify = true

    if (!std::is_floating_point_v<T>)
        return std::round(result);
    return result;
}

template<class T>
T SquareMatrix<T>::detImpl()
{
    if (!std::is_floating_point_v<T>)
    {
        SquareMatrix<fp_t> mat = copy<fp_t>();
        return std::round(mat.det(true));
    }
    lazyGauss();
    return this->trace();
}

template<class T>
void SquareMatrix<T>::lazyGauss()
{
    size_t swapsCount = 0;

    for (size_t col = 0; col < size_; ++col)
    {
        // Next line index is equal to col.
        size_t nextLineIndex = seekNextLine(col);
        if (nextLineIndex == size_)
            return;
        if (nextLineIndex != col)
        {
            Line::swap(data_[col], data_[nextLineIndex]);
            ++swapsCount;
        }
        T leadingValue = data_[col].at(col);

        for (size_t line = col + 1; line < size_; ++line)
        {
            fp_t curLineLeadingValue = data_[line].at(col);

            double ratio = -curLineLeadingValue / leadingValue;
            data_[line].MAdd(data_[col], ratio);
        }
    }

    if (swapsCount % 2)
        at(0) = -at(0);
}

} // namespace linear

#endif
