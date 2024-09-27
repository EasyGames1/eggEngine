#ifndef ENGINE_SOURCES_TYPE_INFO_INTERNAL_FILE_FUNCTION_BOUNDS_H
#define ENGINE_SOURCES_TYPE_INFO_INTERNAL_FILE_FUNCTION_BOUNDS_H

#include <cstddef>
#include <source_location>
#include <string_view>

namespace egg::Types::Internal
{
    template <typename>
    [[nodiscard]] consteval std::string_view GetFunctionName() noexcept
    {
        return std::source_location::current().function_name();
    }

    struct FunctionBounds
    {
        constexpr FunctionBounds(const std::size_t BeginTypeName, const std::size_t RestAfterTypeName)
            : BeginTypeName { BeginTypeName },
              RestAfterTypeName { RestAfterTypeName }
        {
        }

        std::size_t BeginTypeName;
        std::size_t RestAfterTypeName;
    };

    [[nodiscard]] consteval FunctionBounds GetFunctionBounds() noexcept
    {
        constexpr std::string_view PrettyFunction { GetFunctionName<long double>() };

        constexpr std::string_view TypeName { "long double" };
        constexpr std::size_t BeginTypeName { PrettyFunction.find(TypeName) };
        constexpr std::size_t EndTypeName { BeginTypeName + TypeName.size() };

        return { BeginTypeName, PrettyFunction.size() - EndTypeName };
    }
}

#endif // ENGINE_SOURCES_TYPE_INFO_INTERNAL_FILE_FUNCTION_BOUNDS_H
