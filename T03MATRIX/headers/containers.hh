
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <initializer_list>
#include <memory>

namespace containers
{

// Exceptions safe dynamical allocated block storage.
template <class T> class VectorStorage
{
    static_assert(std::is_nothrow_move_constructible<T>::value ||
                  std::is_copy_constructible<T>::value);

  protected:
    size_t used_ = 0;
    T *data_ = nullptr;

    VectorStorage() = default;

    VectorStorage(size_t allocSize) :
        data_{static_cast<T *>(::operator new(sizeof(T) * allocSize))}
    {
    }

    VectorStorage(VectorStorage &&toMove) noexcept
    {
        std::swap(used_, toMove.used_);
        std::swap(data_, toMove.data_);
    }

    VectorStorage &operator=(VectorStorage &&toMove) noexcept
    {
        std::swap(used_, toMove.used_);
        std::swap(data_, toMove.data_);
        return *this;
    }

    VectorStorage(const VectorStorage &) = delete;
    VectorStorage &operator=(const VectorStorage &) = delete;

    ~VectorStorage()
    {
        for (size_t i = 0; i < used_; ++i)
            (data_ + i)->~T();
        ::operator delete(data_);
    }

    void moveOrCopyT(T *dest, T&src)
    {
        if (std::is_nothrow_move_constructible<T>::value)
            new (dest) T(std::move(src));
        else
            new (dest) T(src);
    }

    void realloc(size_t newSize)
    {
        VectorStorage<T> newStorage(newSize);

        used_ = std::min(newSize, used_);

        for (size_t i = 0, last = used_; i < last; ++i)
            moveOrCopyT(newStorage.data_ + i, data_[i]);

        *this = std::move(newStorage);
    }
};

// Dynamical array with elements access.
template <class T> class Vector final : private VectorStorage<T>
{
    using VectorStorage<T>::used_;
    using VectorStorage<T>::data_;
    using VectorStorage<T>::VectorStorage;
    using VectorStorage<T>::realloc;

    size_t allocated_ = 0;

  public:
    Vector(Vector &&) = default;
    Vector &operator=(Vector &&) = default;

    Vector() = default;

    Vector(size_t initSize, const T &initVal = T{}) :
        VectorStorage<T>{initSize}, allocated_(initSize)
    {
        for (; used_ < allocated_; ++used_)
            new (data_ + used_) T{initVal};
    }

    Vector(const Vector &toCopy) :
        VectorStorage<T>{toCopy.used_}, allocated_{toCopy.used_}
    {
        for (; used_ < allocated_; ++used_)
            new (data_ + used_) T{toCopy.data_[used_]};
    }

    Vector &operator=(const Vector &toCopy)
    {
        if (&toCopy == this)
            return *this;

        Vector tmp{toCopy};
        return *this = std::move(tmp);
    }

    ~Vector() = default;

    void pushBack(const T &value)
    {
        if (used_ == allocated_)
            realloc(allocated_ = 2 * allocated_ + 1);

        new(data_ + used_) T{value};
        used_++;
    }

    T &operator[](size_t id) noexcept
    {
        return data_[id];
    }

    const T &operator[](size_t id) const noexcept
    {
        return data_[id];
    }

    size_t size() const noexcept
    {
        return used_;
    }
};

} // namespace containers
