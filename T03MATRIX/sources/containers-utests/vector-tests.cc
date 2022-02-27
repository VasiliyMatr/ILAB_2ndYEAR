
#include "containers.hh"
#include <gtest/gtest.h>

namespace containers
{

namespace
{

// Throws std::bad_alloc in every N_ constructor.
// And allows to check number of created objects.
class TestT final
{
    // Excepts throw period.
    static size_t N_;
    static size_t objsCounter_;

  public:
    TestT()
    {
        if ((objsCounter_ + 1) % N_ == 0)
            throw std::bad_alloc();
        ++objsCounter_;
    }
    TestT(const TestT &) : TestT()
    {
    }
    ~TestT()
    {
        --objsCounter_;
    }

    static size_t numOfObjects()
    {
        return objsCounter_;
    }

    static void setN(size_t N) noexcept
    {
        N_ = N;
    }
};

size_t TestT::N_ = 1;
size_t TestT::objsCounter_ = 0;

using TestVec = Vector<TestT>;

// Function to check exceptions safety.
// Returns false for succeed check.
template <class Callable> bool checkExceptSafety(Callable callable)
{
    // std::cout << "Callable " << typeid(Callable).name() << ":" << std::endl;
    try
    {
        callable();
    }
    catch (const std::bad_alloc &)
    {
        // std::cout << "Caught exception!" << std::endl;
        return !callable.isOk();
    }

    // std::cout << "No exceptions here!" << std::endl;
    return true;
}

// Interface for all exception safety tests callables.
struct IExceptSafetyTestCallable
{
    virtual ~IExceptSafetyTestCallable()
    {
    }

    virtual void operator()() = 0;
    virtual bool isOk() const
    {
        return !TestT::numOfObjects();
    }
};

class CallInitializerListCtor final : public IExceptSafetyTestCallable
{
    static constexpr size_t N_VAL_ = 5;

  public:
    void operator()() override
    {
        TestT::setN(N_VAL_);
        TestVec v = {TestT(), TestT(), TestT()};
    }
};

class ExceptSafetyTestCallableWithCustomN : public IExceptSafetyTestCallable
{
  protected:
    const size_t N_;
    ExceptSafetyTestCallableWithCustomN(size_t N) : N_{N}
    {
    }
};

struct CallCtorFromSize final : public ExceptSafetyTestCallableWithCustomN
{
    CallCtorFromSize(size_t N) : ExceptSafetyTestCallableWithCustomN{N}
    {
    }
    void operator()() override
    {
        TestT::setN(N_);
        TestVec v(N_);
    }
};

struct CallCtorFromSizeAndT final : public ExceptSafetyTestCallableWithCustomN
{
    CallCtorFromSizeAndT(size_t N) : ExceptSafetyTestCallableWithCustomN{N}
    {
    }
    void operator()() override
    {
        TestT::setN(N_);
        TestVec(N_ - 1, TestT());
    }
};

class CallPushNPop final : public ExceptSafetyTestCallableWithCustomN
{
    TestVec v_{};
    size_t sizeBeforeThrow_ = 0;

  public:
    CallPushNPop(size_t N) : ExceptSafetyTestCallableWithCustomN{N}
    {
    }
    void operator()() override
    {
        TestT::setN(N_);

        for (size_t i = 0, N = N_ / 2; i < N; ++i)
        {
            sizeBeforeThrow_ = v_.size();
            v_.push(TestT());
        }
        for (size_t i = 0, N = N_ / 3; i < N; ++i)
            v_.pop();
        for (size_t i = 0, N = N_; i < N; ++i)
        {
            sizeBeforeThrow_ = v_.size();
            v_.push(TestT());
        }
    }
    bool isOk() const override
    {
        // TestVec should be in the same state as it was before throw.
        return v_.size() == sizeBeforeThrow_;
    }
};

} // namespace

TEST(VectorTests, ExceptsSafetyTests)
{
    ASSERT_FALSE(checkExceptSafety(CallInitializerListCtor()));

    for (auto N : {7, 188, 256, 993, 182948})
    {
        // std::cout << N << std::endl;

        ASSERT_FALSE(checkExceptSafety(CallCtorFromSize(N)));
        ASSERT_FALSE(checkExceptSafety(CallCtorFromSizeAndT(N)));
        ASSERT_FALSE(checkExceptSafety(CallPushNPop(N)));
    }
}

} // namespace containers
