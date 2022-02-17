
#include <cstdlib>
#include <utility>
#include <algorithm>
#include <initializer_list>
#include <memory>

#ifndef CONTAINERS_HH_INCL
#define CONTAINERS_HH_INCL

namespace containers
{

// Exceptions safe dynamical allocated block storage.
template <class T> class VectorStorage
{
  protected:
    size_t allocated_ = 0;
    size_t size_ = 0;
    T *data_ = nullptr;

    VectorStorage() = default;

    VectorStorage(size_t allocSize) : allocated_(allocSize),
        data_{static_cast<T *>(::operator new(sizeof(T) * allocSize))}
    {
    }

    VectorStorage(VectorStorage &&toMove) noexcept
    {
        std::swap(allocated_, toMove.allocated_);
        std::swap(size_, toMove.size_);
        std::swap(data_, toMove.data_);
    }

    VectorStorage &operator=(VectorStorage &&toMove) noexcept
    {
        std::swap(allocated_, toMove.allocated_);
        std::swap(size_, toMove.size_);
        std::swap(data_, toMove.data_);
        return *this;
    }

    VectorStorage(const VectorStorage &) = delete;
    VectorStorage &operator=(const VectorStorage &) = delete;

    ~VectorStorage()
    {
        for (size_t i = 0; i < size_; ++i)
            (data_ + i)->~T();
        ::operator delete(data_);
    }
};

// Dynamical array with elements access.
template <class T> class Vector final : private VectorStorage<T>
{
    static_assert(std::is_nothrow_move_constructible<T>::value ||
        std::is_copy_constructible<T>::value,
        "T should be copy constructible or nothrow moveable.");

    using VectorStorage<T>::VectorStorage;
    using VectorStorage<T>::allocated_;
    using VectorStorage<T>::size_;
    using VectorStorage<T>::data_;

  public:
    Vector(Vector &&) = default;
    Vector &operator=(Vector &&) = default;
    Vector() = default;

    template<class Arg = T, std::enable_if_t<std::is_copy_constructible_v<Arg>, int> = 0>
    Vector(std::initializer_list<T> l) : VectorStorage<T> {l.size()}
    {
        for (auto it = l.begin(), end = l.end(); it != end; ++it, ++size_)
            new (data_ + size_) T(*it);
    }
    template<class Arg = T, std::enable_if_t<std::is_copy_constructible_v<Arg>, int> = 0>
    Vector(size_t size, const T &toCopy) : VectorStorage<T> {size}
    {
        static_assert(std::is_same_v<Arg, T>, "Arg should be default type (T).");

        for (size_t i = 0; i < size_; ++i)
            new (data_ + i) T(toCopy);
    }
    template<class Arg = T, std::enable_if_t<std::is_default_constructible_v<Arg>, int> = 0>
    Vector(size_t size) : VectorStorage<T> {size}
    {
        static_assert(std::is_same_v<Arg, T>, "Arg should be default type (T).");

        for (size_t i = 0; i < size_; ++i)
            new (data_ + i) T{};
    }

    ~Vector() = default;

  private:
    template<class Arg = T, std::enable_if_t<std::is_nothrow_move_constructible_v<Arg>, int> = 0>
    void moveOrCopyT(T *dest, T &src)
    {
        static_assert(std::is_same_v<Arg, T>, "Arg should be default type (T).");

        new (dest) T(std::move(src));
    }
    template<class Arg = T, std::enable_if_t<!std::is_nothrow_move_constructible_v<Arg>, int> = 0>
    void moveOrCopyT(T *dest, T &src)
    {
        static_assert(std::is_same_v<Arg, T>, "Arg should be default type (T).");

        new (dest) T(src);
    }
    class dummy final{};
    // Ctor for reallocation impl with dummy parameter.
    Vector(size_t newAllocated, dummy) : VectorStorage<T>{newAllocated}
    {
    }
    void realloc(size_t newAllocated)
    {
        size_t newSize = std::min(newAllocated, size_);
        Vector newVector(newAllocated, dummy{});
        newVector.size_ = newSize;

        for (size_t i = 0; i < newSize; ++i)
            moveOrCopyT(newVector.data_ + i, data_[i]);

        *this = std::move(newVector);
    }

  public:
    template<class Arg>
    void push(Arg &&value)
    {
        if (size_ == allocated_)
            realloc(2 * allocated_ + 1);

        new(data_ + size_) T{std::forward<Arg>(value)};
        ++size_;
    }
    void pop()
    {
        if (size_ == 0) return;
        if (size_ * 4 < allocated_)
            realloc(size_);

        (data_ + size_ - 1)->~T();
        --size_;
    }

    T &back() noexcept
    {
        return *(data_ + size_ - 1);
    }
    const T &back() const noexcept
    {
        return *(data_ + size_ - 1);
    }

    T &front() noexcept
    {
        return *data_;
    }
    const T &front() const noexcept
    {
        return *data_;
    }

    class iterator final
    {
        friend Vector;
        T const *ptr_;

        iterator(T *ptr) noexcept : ptr_(ptr) {}

      public:
        iterator(const iterator&) = default;
        iterator &operator=(const iterator&) = default;

        T &operator*() const noexcept
        {
            return *ptr_;
        }

        iterator &operator++() noexcept
        {
            ++ptr_;
            return *this;
        }
        iterator operator++(int) noexcept
        {
            iterator copy{*this};
            ++ptr_;
            return copy;
        }
        iterator &operator--() noexcept
        {
            --ptr_;
            return *this;
        }
        iterator operator--(int) noexcept
        {
            iterator copy{*this};
            --ptr_;
            return copy;
        }

        bool operator==(const iterator& second) const noexcept
        {
            return ptr_ == second.ptr_;
        }
    };

    class const_iterator final
    {
        iterator it_;

      public:
        const_iterator(const iterator &it) noexcept : it_(it) {}
        const_iterator(const const_iterator&) = default;
        const_iterator &operator=(const const_iterator&) = default;

        const T &operator*() const noexcept
        {
            return *it_;
        }

        const_iterator &operator++()
        {
            ++it_;
            return *this;
        }
        const_iterator operator++(int)
        {
            return it_++;
        }
        const_iterator &operator--()
        {
            --it_;
            return *this;
        }
        const_iterator operator--(int)
        {
            return it_--;
        }
    };

    iterator begin() noexcept
    {
        return iterator(data_);
    }
    iterator end() noexcept
    {
        return iterator(data_ + size_);
    }
    const_iterator begin() const noexcept
    {
        return const_iterator(data_);
    }
    const_iterator end() const noexcept
    {
        return const_iterator(data_ + size_);
    }
    const_iterator cbegin() const noexcept
    {
        return const_iterator(data_);
    }
    const_iterator cend() const noexcept
    {
        return const_iterator(data_ + size_);
    }

    T &operator[](size_t id) noexcept
    {
        return data_[id];
    }
    const T &operator[](size_t id) const noexcept
    {
        return data_[id];
    }

    size_t allocated() const noexcept
    {
        return allocated_;
    }
    size_t size() const noexcept
    {
        return size_;
    }
};

} // namespace containers

#endif // #ifndef CONTAINERS_HH_INCL
