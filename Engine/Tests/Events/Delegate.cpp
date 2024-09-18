#include "Events/Delegate.h"
#include "../Single.h"

#include <gtest/gtest.h>

#include <array>

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

enum class FunctionKind
{
    Free,
    Member,
    Lambda,
    Functor
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
struct PointerTraits<void(), IsNoExcept, FunctionKind::Member> :
    Traits<&Helper::VoidNoArgs<IsNoExcept>>
{
};

template <bool IsNoExcept>
struct PointerTraits<void(int), IsNoExcept, FunctionKind::Member> :
    Traits<&Helper::VoidInt<IsNoExcept>>
{
};

template <bool IsNoExcept>
struct PointerTraits<void(Helper&), IsNoExcept, FunctionKind::Member> :
    Traits<&Helper::VoidObject<IsNoExcept>>
{
};

template <bool IsNoExcept>
struct PointerTraits<void(Helper&, int), IsNoExcept, FunctionKind::Member> :
    Traits<&Helper::VoidObjectInt<IsNoExcept>>
{
};

template <bool IsNoExcept>
struct PointerTraits<void(), IsNoExcept, FunctionKind::Lambda> :
    Traits<+[]()-> void
        {
        }
    >
{
};

template <bool IsNoExcept>
struct PointerTraits<void(int), IsNoExcept, FunctionKind::Lambda> :
    Traits<+[](const int) -> void
        {
        }
    >
{
};

template <bool IsNoExcept>
struct PointerTraits<void(Helper&), IsNoExcept, FunctionKind::Lambda> :
    Traits<+[](const Helper&) -> void
    {
    }>
{
};

template <bool IsNoExcept>
struct PointerTraits<void(Helper&, int), IsNoExcept, FunctionKind::Lambda> :
    Traits<+[](Helper&, const int)-> void
        {
        }
    >
{
};

template <bool IsNoExcept>
struct PointerTraits<void(), IsNoExcept, FunctionKind::Functor> :
    Traits<&FunctorsHelper<IsNoExcept>::FunctorVoidNoArgs>
{
};

template <bool IsNoExcept>
struct PointerTraits<void(int), IsNoExcept, FunctionKind::Functor> :
    Traits<&FunctorsHelper<IsNoExcept>::FunctorVoidInt>
{
};

template <bool IsNoExcept>
struct PointerTraits<void(Helper&), IsNoExcept, FunctionKind::Functor> :
    Traits<&FunctorsHelper<IsNoExcept>::FunctorVoidObject>
{
};

template <bool IsNoExcept>
struct PointerTraits<void(Helper&, int), IsNoExcept, FunctionKind::Functor> :
    Traits<&FunctorsHelper<IsNoExcept>::FunctorVoidObjectInt>
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

    static constexpr auto FreePointer { PointerTraits<FunctionType, false, FunctionKind::Free>::Pointer };
    static constexpr auto FreePointerNoExcept { PointerTraits<FunctionType, true, FunctionKind::Free>::Pointer };

    static constexpr auto MemberPointer { PointerTraits<FunctionType, false, FunctionKind::Member>::Pointer };
    static constexpr auto MemberPointerNoExcept { PointerTraits<FunctionType, true, FunctionKind::Member>::Pointer };

    static constexpr auto Lambda { PointerTraits<FunctionType, false, FunctionKind::Lambda>::Pointer };
    static constexpr auto LambdaNoExcept { PointerTraits<FunctionType, true, FunctionKind::Lambda>::Pointer };

    static constexpr auto FunctorPointer { PointerTraits<FunctionType, false, FunctionKind::Functor>::Pointer };
    static constexpr auto FunctorPointerNoExcept { PointerTraits<FunctionType, true, FunctionKind::Functor>::Pointer };
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
    this->Delegate.template Connect<this->FreePointer>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->FreePointerNoExcept>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();


    Helper HelperObject;
    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberPointer>(HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberPointer>(&HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberPointerNoExcept>(HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberPointerNoExcept>(&HelperObject);
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


    FunctorsHelper<false> FunctorHelperObject;
    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->FunctorPointer>(FunctorHelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->FunctorPointer>(&FunctorHelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    FunctorsHelper<true> FunctorHelperNoExceptObject;
    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->FunctorPointerNoExcept>(FunctorHelperNoExceptObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->FunctorPointerNoExcept>(&FunctorHelperNoExceptObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
}

TYPED_TEST(DelegateTest, Constructors)
{
    egg::Events::Delegate DelegateFreePointer { egg::Events::ConnectionArgument<this->FreePointer> {} };
    EXPECT_TRUE(DelegateFreePointer);

    egg::Events::Delegate DelegateFreePointerNoExcept { egg::Events::ConnectionArgument<this->FreePointerNoExcept> {} };
    EXPECT_TRUE(DelegateFreePointerNoExcept);


    Helper HelperObject;
    egg::Events::Delegate DelegateMemberPointer { egg::Events::ConnectionArgument<this->MemberPointer> {}, HelperObject };
    EXPECT_TRUE(DelegateMemberPointer);

    egg::Events::Delegate DelegateMemberPointerWithPointer {
        egg::Events::ConnectionArgument<this->MemberPointer> {}, &HelperObject
    };
    EXPECT_TRUE(DelegateMemberPointerWithPointer);

    egg::Events::Delegate DelegateMemberPointerNoExcept {
        egg::Events::ConnectionArgument<this->MemberPointerNoExcept> {}, HelperObject
    };
    EXPECT_TRUE(DelegateMemberPointerNoExcept);

    egg::Events::Delegate DelegateMemberPointerNoExceptWithPointer {
        egg::Events::ConnectionArgument<this->MemberPointerNoExcept> {}, &HelperObject
    };
    EXPECT_TRUE(DelegateMemberPointerNoExceptWithPointer);


    egg::Events::Delegate DelegateLambda { egg::Events::ConnectionArgument<this->Lambda> {} };
    EXPECT_TRUE(DelegateLambda);

    egg::Events::Delegate DelegateLambdaNoExcept { egg::Events::ConnectionArgument<this->LambdaNoExcept> {} };
    EXPECT_TRUE(DelegateLambdaNoExcept);


    FunctorsHelper<false> FunctorHelperObject;
    egg::Events::Delegate DelegateFunctorPointer {
        egg::Events::ConnectionArgument<this->FunctorPointer> {}, FunctorHelperObject
    };
    EXPECT_TRUE(DelegateFunctorPointer);

    egg::Events::Delegate DelegateFunctorPointerWithPointer {
        egg::Events::ConnectionArgument<this->FunctorPointer> {}, &FunctorHelperObject
    };
    EXPECT_TRUE(DelegateFunctorPointerWithPointer);

    FunctorsHelper<true> FunctorHelperNoExceptObject;
    egg::Events::Delegate DelegateFunctorPointerNoExcept {
        egg::Events::ConnectionArgument<this->FunctorPointerNoExcept> {}, FunctorHelperNoExceptObject
    };
    EXPECT_TRUE(DelegateFunctorPointerNoExcept);

    egg::Events::Delegate DelegateFunctorPointerNoExceptWithPointer {
        egg::Events::ConnectionArgument<this->FunctorPointerNoExcept> {}, &FunctorHelperNoExceptObject
    };
    EXPECT_TRUE(DelegateFunctorPointerNoExceptWithPointer);
}

TYPED_TEST(DelegateTest, Reset)
{
    this->Delegate.template Connect<this->FreePointer>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);

    this->Delegate.template Connect<this->FreePointerNoExcept>();
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);


    Helper HelperObject;
    this->Delegate.template Connect<this->MemberPointer>(HelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);

    this->Delegate.template Connect<this->MemberPointerNoExcept>(HelperObject);
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


    FunctorsHelper<false> FunctorHelperObject;
    this->Delegate.template Connect<this->FunctorPointer>(FunctorHelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);

    FunctorsHelper<true> FunctorHelperNoExceptObject;
    this->Delegate.template Connect<this->FunctorPointerNoExcept>(FunctorHelperNoExceptObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);
}
