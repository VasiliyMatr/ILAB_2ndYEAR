
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

    size_t size() const noexcept
    {
        return size_;
    }
};

} // namespace containers
