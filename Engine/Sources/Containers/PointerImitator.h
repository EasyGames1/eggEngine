#ifndef ENGINE_SOURCES_CONTAINERS_FILE_POINTER_IMITATOR_H
#define ENGINE_SOURCES_CONTAINERS_FILE_POINTER_IMITATOR_H

#include <type_traits>
#include <utility>

namespace egg::Containers
{
    template <typename Type>
    class PointerImitator final
    {
    public:
        using ValueType = Type;
        using Pointer = Type*;
        using Reference = Type&;


        constexpr explicit PointerImitator(ValueType&& Value) noexcept (std::is_nothrow_move_constructible_v<ValueType>)
            : Value { std::move(Value) }
        {
        }

        [[nodiscard]] constexpr Pointer operator->() noexcept
        {
            return std::addressof(Value);
        }

        [[nodiscard]] constexpr Reference operator*() noexcept
        {
            return Value;
        }

    private:
        Type Value;
    };
}


#endif // ENGINE_SOURCES_CONTAINERS_FILE_POINTER_IMITATOR_H
