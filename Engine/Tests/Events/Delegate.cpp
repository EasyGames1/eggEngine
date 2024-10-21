#include "Events/Delegate.h"

#include <gtest/gtest.h>

namespace Functors
{
    template <bool IsNoExcept>
    struct VoidNoArgs
    {
        void operator()() noexcept(IsNoExcept)
        {
        }
    };

    template <bool IsNoExcept>
    struct VoidInt
    {
        void operator()(const int) noexcept(IsNoExcept)
        {
        }
    };

    template <bool IsNoExcept>
    struct VoidObject
    {
        void operator()() noexcept(IsNoExcept)
        {
        }
    };

    template <bool IsNoExcept>
    struct VoidObjectInt
    {
        void operator()(const int) noexcept(IsNoExcept)
        {
        }
    };
}

struct Helper
{
    template <bool IsNoExcept>
    void VoidNoArgs() noexcept(IsNoExcept)
    {
        ++Value;
    }

    template <bool IsNoExcept>
    void VoidInt(const int Value) noexcept(IsNoExcept)
    {
        this->Value = Value;
    }

    template <bool IsNoExcept>
    void VoidObject() noexcept(IsNoExcept)
    {
        VoidNoArgs<IsNoExcept>();
    }

    template <bool IsNoExcept>
    void VoidObjectInt(const int Value) noexcept(IsNoExcept)
    {
        return VoidInt<IsNoExcept>(Value);
    }

    int Value {};
};

template <bool IsNoExcept>
struct FunctorsHelper
{
    Functors::VoidNoArgs<IsNoExcept> FunctorVoidNoArgs {};
    Functors::VoidInt<IsNoExcept> FunctorVoidInt {};
    Functors::VoidObject<IsNoExcept> FunctorVoidObject {};
    Functors::VoidObjectInt<IsNoExcept> FunctorVoidObjectInt {};
};

namespace FreeFunctions
{
    template <bool IsNoExcept>
    static void VoidNoArgs() noexcept(IsNoExcept)
    {
    }

    template <bool IsNoExcept>
    static void VoidInt(const int) noexcept(IsNoExcept)
    {
    }

    template <bool IsNoExcept>
    static void VoidObject(Helper&) noexcept(IsNoExcept)
    {
    }

    template <bool IsNoExcept>
    static void VoidObjectInt(Helper&, const int) noexcept(IsNoExcept)
    {
    }
}

namespace Lambdas
{
    template <bool IsNoExcept>
    [[nodiscard]] consteval static auto VoidNoArgs() noexcept(IsNoExcept)
    {
        return []() -> void
        {
        };
    }

    template <bool IsNoExcept>
    [[nodiscard]] consteval static auto VoidInt() noexcept(IsNoExcept)
    {
        return [](int) -> void
        {
        };
    }

    template <bool IsNoExcept>
    [[nodiscard]] consteval static auto VoidObject() noexcept(IsNoExcept)
    {
        return [](Helper&) -> void
        {
        };
    }

    template <bool IsNoExcept>
    [[nodiscard]] consteval static auto VoidObjectInt() noexcept(IsNoExcept)
    {
        return [](Helper&, int) -> void
        {
        };
    }
}

enum class FunctionKind
{
    Free,
    MemberFunction,
    MemberValue,
    MemberFunctor,
    Lambda,
};

template <auto Candidate>
struct Traits
{
    static constexpr auto Pointer { Candidate };
};

template <typename FunctionType, bool IsNoExcept, FunctionKind Kind>
struct PointerTraits;


template <bool IsNoExcept>
struct PointerTraits<void(), IsNoExcept, FunctionKind::Free> :
    Traits<&FreeFunctions::VoidNoArgs<IsNoExcept>>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(int), IsNoExcept, FunctionKind::Free> :
    Traits<&FreeFunctions::VoidInt<IsNoExcept>>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(Helper&), IsNoExcept, FunctionKind::Free> :
    Traits<&FreeFunctions::VoidObject<IsNoExcept>>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(Helper&, int), IsNoExcept, FunctionKind::Free> :
    Traits<&FreeFunctions::VoidObjectInt<IsNoExcept>>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(), IsNoExcept, FunctionKind::MemberFunction> :
    Traits<&Helper::VoidNoArgs<IsNoExcept>>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(int), IsNoExcept, FunctionKind::MemberFunction> :
    Traits<&Helper::VoidInt<IsNoExcept>>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(Helper&), IsNoExcept, FunctionKind::MemberFunction> :
    Traits<&Helper::VoidObject<IsNoExcept>>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(Helper&, int), IsNoExcept, FunctionKind::MemberFunction> :
    Traits<&Helper::VoidObjectInt<IsNoExcept>>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(), IsNoExcept, FunctionKind::MemberValue> :
    Traits<&Helper::Value>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(int), IsNoExcept, FunctionKind::MemberValue> :
    Traits<&Helper::Value>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(Helper&), IsNoExcept, FunctionKind::MemberValue> :
    Traits<&Helper::Value>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(Helper&, int), IsNoExcept, FunctionKind::MemberValue> :
    Traits<&Helper::Value>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(), IsNoExcept, FunctionKind::MemberFunctor> :
    Traits<&FunctorsHelper<IsNoExcept>::FunctorVoidNoArgs>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(int), IsNoExcept, FunctionKind::MemberFunctor> :
    Traits<&FunctorsHelper<IsNoExcept>::FunctorVoidInt>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(Helper&), IsNoExcept, FunctionKind::MemberFunctor> :
    Traits<&FunctorsHelper<IsNoExcept>::FunctorVoidObject>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(Helper&, int), IsNoExcept, FunctionKind::MemberFunctor> :
    Traits<&FunctorsHelper<IsNoExcept>::FunctorVoidObjectInt>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(), IsNoExcept, FunctionKind::Lambda>
    : Traits<+Lambdas::VoidNoArgs<IsNoExcept>()>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(int), IsNoExcept, FunctionKind::Lambda> :
    Traits<+Lambdas::VoidInt<IsNoExcept>()>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(Helper&), IsNoExcept, FunctionKind::Lambda> :
    Traits<+Lambdas::VoidObject<IsNoExcept>()>
{
};


template <bool IsNoExcept>
struct PointerTraits<void(Helper&, int), IsNoExcept, FunctionKind::Lambda> :
    Traits<+Lambdas::VoidObjectInt<IsNoExcept>()>
{
};


template <typename FunctionType>
class DelegateTest : public DelegateTest<std::pair<FunctionType, FunctionType>>
{
};


template <typename DelegatePointerType, typename FunctionPointerType>
class DelegateTest<std::pair<DelegatePointerType, FunctionPointerType>> : public testing::Test
{
protected:
    using DelegateType = std::remove_pointer_t<DelegatePointerType>;
    using FunctionType = std::remove_pointer_t<FunctionPointerType>;

    egg::Events::Delegate<DelegateType> Delegate;

    static constexpr auto Free { PointerTraits<FunctionType, false, FunctionKind::Free>::Pointer };
    static constexpr auto FreeNoExcept { PointerTraits<FunctionType, true, FunctionKind::Free>::Pointer };

    static constexpr auto MemberFunction { PointerTraits<FunctionType, false, FunctionKind::MemberFunction>::Pointer };
    static constexpr auto MemberFunctionNoExcept { PointerTraits<FunctionType, true, FunctionKind::MemberFunction>::Pointer };

    static constexpr auto MemberValue { PointerTraits<FunctionType, false, FunctionKind::MemberValue>::Pointer };

    static constexpr auto MemberFunctor { PointerTraits<FunctionType, false, FunctionKind::MemberFunctor>::Pointer };
    static constexpr auto MemberFunctorNoExcept { PointerTraits<FunctionType, true, FunctionKind::MemberFunctor>::Pointer };

    static constexpr auto Lambda { PointerTraits<FunctionType, false, FunctionKind::Lambda>::Pointer };
    static constexpr auto LambdaNoExcept { PointerTraits<FunctionType, true, FunctionKind::Lambda>::Pointer };
};

using FunctionTypes = testing::Types<
    void(),
    void(int),
    void(Helper&),
    void(Helper&, int),
    std::pair<void(*)(int, int), void(*)()>,
    std::pair<void(*)(int, int), void(*)(int)>,
    std::pair<void(*)(Helper&, int), void(*)()>,
    std::pair<void(*)(Helper&, int), void(*)(int)>
>;
TYPED_TEST_SUITE(DelegateTest, FunctionTypes);


TYPED_TEST(DelegateTest, Connect)
{
    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->Free>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->FreeNoExcept>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();


    Helper HelperObject;
    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberFunction>(HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberFunction>(&HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberFunctionNoExcept>(HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberFunctionNoExcept>(&HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();


    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberValue>(HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberValue>(&HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();


    FunctorsHelper<false> FunctorHelperObject;
    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberFunctor>(FunctorHelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberFunctor>(&FunctorHelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    FunctorsHelper<true> FunctorHelperNoExceptObject;
    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberFunctorNoExcept>(FunctorHelperNoExceptObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberFunctorNoExcept>(&FunctorHelperNoExceptObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();


    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->Lambda>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->LambdaNoExcept>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
}


TYPED_TEST(DelegateTest, Constructors)
{
    egg::Events::Delegate DelegateFree { egg::Events::ConnectionArgument<this->Free> };
    EXPECT_TRUE(DelegateFree);

    egg::Events::Delegate DelegateFreeNoExcept { egg::Events::ConnectionArgument<this->FreeNoExcept> };
    EXPECT_TRUE(DelegateFreeNoExcept);


    Helper HelperObject;
    egg::Events::Delegate DelegateMemberFunction { egg::Events::ConnectionArgument<this->MemberFunction>, HelperObject };
    EXPECT_TRUE(DelegateMemberFunction);

    egg::Events::Delegate DelegateMemberFunctionWithPointer {
        egg::Events::ConnectionArgument<this->MemberFunction>, &HelperObject
    };
    EXPECT_TRUE(DelegateMemberFunctionWithPointer);

    egg::Events::Delegate DelegateMemberFunctionNoExcept {
        egg::Events::ConnectionArgument<this->MemberFunctionNoExcept>, HelperObject
    };
    EXPECT_TRUE(DelegateMemberFunctionNoExcept);

    egg::Events::Delegate DelegateMemberFunctionNoExceptWithPointer {
        egg::Events::ConnectionArgument<this->MemberFunctionNoExcept>, &HelperObject
    };
    EXPECT_TRUE(DelegateMemberFunctionNoExceptWithPointer);


    egg::Events::Delegate DelegateMemberValue {
        egg::Events::ConnectionArgument<this->MemberValue>, HelperObject
    };
    EXPECT_TRUE(DelegateMemberValue);

    egg::Events::Delegate DelegateMemberValueWithPointer {
        egg::Events::ConnectionArgument<this->MemberValue>, &HelperObject
    };
    EXPECT_TRUE(DelegateMemberValueWithPointer);


    FunctorsHelper<false> FunctorHelperObject;
    egg::Events::Delegate DelegateMemberFunctor {
        egg::Events::ConnectionArgument<this->MemberFunctor>, FunctorHelperObject
    };
    EXPECT_TRUE(DelegateMemberFunctor);

    egg::Events::Delegate DelegateMemberFunctorWithPointer {
        egg::Events::ConnectionArgument<this->MemberFunctor>, &FunctorHelperObject
    };
    EXPECT_TRUE(DelegateMemberFunctorWithPointer);

    FunctorsHelper<true> FunctorHelperNoExceptObject;
    egg::Events::Delegate DelegateMemberFunctorNoExcept {
        egg::Events::ConnectionArgument<this->MemberFunctorNoExcept>, FunctorHelperNoExceptObject
    };
    EXPECT_TRUE(DelegateMemberFunctorNoExcept);

    egg::Events::Delegate DelegateMemberFunctorNoExceptWithPointer {
        egg::Events::ConnectionArgument<this->MemberFunctorNoExcept>, &FunctorHelperNoExceptObject
    };
    EXPECT_TRUE(DelegateMemberFunctorNoExceptWithPointer);


    egg::Events::Delegate DelegateLambda { egg::Events::ConnectionArgument<this->Lambda> };
    EXPECT_TRUE(DelegateLambda);

    egg::Events::Delegate DelegateLambdaNoExcept { egg::Events::ConnectionArgument<this->LambdaNoExcept> };
    EXPECT_TRUE(DelegateLambdaNoExcept);
}


TYPED_TEST(DelegateTest, Reset)
{
    this->Delegate.template Connect<this->Free>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);

    this->Delegate.template Connect<this->FreeNoExcept>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);


    Helper HelperObject;
    this->Delegate.template Connect<this->MemberFunction>(HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);

    this->Delegate.template Connect<this->MemberFunctionNoExcept>(HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);


    this->Delegate.template Connect<this->MemberValue>(HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);


    FunctorsHelper<false> FunctorHelperObject;
    this->Delegate.template Connect<this->MemberFunctor>(FunctorHelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);

    FunctorsHelper<true> FunctorHelperNoExceptObject;
    this->Delegate.template Connect<this->MemberFunctorNoExcept>(FunctorHelperNoExceptObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);


    this->Delegate.template Connect<this->Lambda>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);

    this->Delegate.template Connect<this->LambdaNoExcept>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);
}
