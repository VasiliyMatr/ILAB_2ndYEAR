
#include <gtest/gtest.h>
#include "containers.hh"

namespace containers
{

// namespace
// {

// Throws std::bad_alloc in every N_ constructor.
// And allows to check if all created objects were destructed.
class ExceptsSafetyChecker final
{
    // Counts number of existent objects of class CountedObject.
    class CountedObject final
    {
        static size_t objsCounter_;

      public:
        CountedObject()
        {
            ++objsCounter_;
        }
        ~CountedObject()
        {
            --objsCounter_;
        }

        static size_t numOfObjects() noexcept
        {
            return objsCounter_;
        }
    };

    // Excepts throw period.
    static size_t N_;

    CountedObject obj_;

  public:
    void ctorAction()
    {
        if (CountedObject::numOfObjects() % N_ == 0)
            throw std::bad_alloc();
    }

    ExceptsSafetyChecker() : obj_{}
    {
        ctorAction();
    }
    ExceptsSafetyChecker(const ExceptsSafetyChecker&) : obj_{}
    {
        ctorAction();
    }

    static bool areAllDestructed() noexcept
    {
        return CountedObject::numOfObjects() == 0;
    }
    static void setN(size_t N) noexcept
    {
        N_ = N;
    }
};

size_t ExceptsSafetyChecker::CountedObject::objsCounter_ = 0;
size_t ExceptsSafetyChecker::N_ = 1;

using TestVec = Vector<ExceptsSafetyChecker>;

// Returns false for succeed check.
template<class Callable>
bool checkExceptSafety(Callable callable)
{
    std::cout << "Callable " << typeid(Callable).name() << ":" << std::endl;
    try {
        callable();
    }
    catch(const std::bad_alloc&) {
        std::cout << "Caught exception!" << std::endl;
        return !ExceptsSafetyChecker::areAllDestructed();
    }

    std::cout << "No exceptions here!" << std::endl;
    return true;
}

class CheckCallable
{
    const size_t N_;

  public:
    CheckCallable(size_t N) : N_{N} {}
    size_t getN() const
    {
        return N_;
    }
    virtual ~CheckCallable() {}
    virtual void operator()() const = 0;
};

struct CallCtorFromSize final : public CheckCallable
{
    CallCtorFromSize(size_t N) : CheckCallable{N} {}
    void operator()() const override
    {
        TestVec v(getN());
    }
};

struct CallCtorFromSizeAndT final : public CheckCallable
{
    CallCtorFromSizeAndT(size_t N) : CheckCallable{N} {}
    void operator()() const override
    {
        TestVec(getN() - 1, ExceptsSafetyChecker());
    }
};

struct CallPushNPop final : public CheckCallable
{
    CallPushNPop(size_t N) : CheckCallable{N} {}
    void operator()() const override
    {
        TestVec t{};

        for (size_t i = 0, N = getN() / 2; i < N; ++i)
            t.push(ExceptsSafetyChecker());
        for (size_t i = 0, N = getN() / 3; i < N; ++i)
            t.pop();
        for (size_t i = 0, N = getN(); i < N; ++i)
            t.push(ExceptsSafetyChecker());
    }
};

// } // namespace

TEST(VectorTests, ExceptsSafetyTests)
{
    std::vector<size_t> testValues = {7, 188, 256, 993};

    for (auto N : testValues)
    {
        ExceptsSafetyChecker::setN(N);

        ASSERT_FALSE (checkExceptSafety<CallCtorFromSize>(CallCtorFromSize(N)));
        ASSERT_FALSE (checkExceptSafety<CallCtorFromSizeAndT>(CallCtorFromSizeAndT(N)));
        ASSERT_FALSE (checkExceptSafety<CallPushNPop>(CallPushNPop(N)));
    }
}

} // namespace containers
