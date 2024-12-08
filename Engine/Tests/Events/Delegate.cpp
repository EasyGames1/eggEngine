#include "../CommonFunctions.h"

#include <Events/ConnectionArgument.h>
#include <Events/Delegate.h>
#include <gtest/gtest.h>


template <typename FunctionType>
class DelegateTest : public DelegateTest<std::pair<FunctionType, FunctionType>>
{
};


template <typename SignaturePointerType, typename FunctionPointerType>
class DelegateTest<std::pair<SignaturePointerType, FunctionPointerType>> : public testing::Test
{
protected:
    using Signature = std::remove_pointer_t<SignaturePointerType>;
    using FunctionSignature = std::remove_pointer_t<FunctionPointerType>;

    egg::Events::Delegate<Signature> Delegate;

    static constexpr auto Free {
        CommonFunctions::PointerTraits<FunctionSignature, false, CommonFunctions::FunctionKind::Free>::Pointer
    };
    static constexpr auto FreeNoExcept {
        CommonFunctions::PointerTraits<FunctionSignature, true, CommonFunctions::FunctionKind::Free>::Pointer
    };


    static constexpr auto MemberFunction {
        CommonFunctions::PointerTraits<FunctionSignature, false, CommonFunctions::FunctionKind::MemberFunction>::Pointer
    };
    static constexpr auto MemberFunctionNoExcept {
        CommonFunctions::PointerTraits<FunctionSignature, true, CommonFunctions::FunctionKind::MemberFunction>::Pointer
    };


    static constexpr auto MemberValue {
        CommonFunctions::PointerTraits<FunctionSignature, false, CommonFunctions::FunctionKind::MemberValue>::Pointer
    };


    static constexpr auto MemberFunctor {
        CommonFunctions::PointerTraits<FunctionSignature, false, CommonFunctions::FunctionKind::MemberFunctor>::Pointer
    };
    static constexpr auto MemberFunctorNoExcept {
        CommonFunctions::PointerTraits<FunctionSignature, true, CommonFunctions::FunctionKind::MemberFunctor>::Pointer
    };


    static constexpr auto Lambda {
        CommonFunctions::PointerTraits<FunctionSignature, false, CommonFunctions::FunctionKind::Lambda>::Pointer
    };
    static constexpr auto LambdaNoExcept {
        CommonFunctions::PointerTraits<FunctionSignature, true, CommonFunctions::FunctionKind::Lambda>::Pointer
    };
};

using FunctionTypes = testing::Types<
    void(),
    void(int),
    void(CommonFunctions::Helper&),
    void(CommonFunctions::Helper&, int),
    std::pair<void(*)(int, int), void(*)()>,
    std::pair<void(*)(int, int), void(*)(int)>,
    std::pair<void(*)(CommonFunctions::Helper&, int), void(*)()>,
    std::pair<void(*)(CommonFunctions::Helper&, int), void(*)(int)>
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


    CommonFunctions::Helper HelperObject;
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


    CommonFunctions::FunctorsHelper<false> FunctorHelperObject;
    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberFunctor>(FunctorHelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    EXPECT_FALSE(this->Delegate);
    this->Delegate.template Connect<this->MemberFunctor>(&FunctorHelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();

    CommonFunctions::FunctorsHelper<true> FunctorHelperNoExceptObject;
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


    CommonFunctions::Helper HelperObject;
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


    CommonFunctions::FunctorsHelper<false> FunctorHelperObject;
    egg::Events::Delegate DelegateMemberFunctor {
        egg::Events::ConnectionArgument<this->MemberFunctor>, FunctorHelperObject
    };
    EXPECT_TRUE(DelegateMemberFunctor);

    egg::Events::Delegate DelegateMemberFunctorWithPointer {
        egg::Events::ConnectionArgument<this->MemberFunctor>, &FunctorHelperObject
    };
    EXPECT_TRUE(DelegateMemberFunctorWithPointer);

    CommonFunctions::FunctorsHelper<true> FunctorHelperNoExceptObject;
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


    CommonFunctions::Helper HelperObject;
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


    CommonFunctions::FunctorsHelper<false> FunctorHelperObject;
    this->Delegate.template Connect<this->MemberFunctor>(FunctorHelperObject);
    EXPECT_TRUE(this->Delegate);
    this->Delegate.Reset();
    EXPECT_FALSE(this->Delegate);

    CommonFunctions::FunctorsHelper<true> FunctorHelperNoExceptObject;
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
