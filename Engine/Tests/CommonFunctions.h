#ifndef ENGINE_TESTS_FILE_COMMON_FUNCTIONS_H
#define ENGINE_TESTS_FILE_COMMON_FUNCTIONS_H

namespace CommonFunctions
{
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
}

#endif // ENGINE_TESTS_FILE_COMMON_FUNCTIONS_H
