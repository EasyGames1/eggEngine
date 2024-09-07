#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_FUNCTION_NAME_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_FUNCTION_NAME_H

#include <source_location>
#include <string_view>

namespace egg::Types::Internal
{
    template <typename>
    [[nodiscard]] consteval std::string_view GetFunctionName() noexcept
    {
        return std::source_location::current().function_name();
    }

    [[nodiscard]] consteval std::pair<std::size_t, std::size_t> GetFunctionBounds() noexcept
    {
        constexpr std::string_view PrettyFunction { GetFunctionName<long double>() };

        constexpr std::string_view TypeName { "long double" };
        constexpr std::size_t BeginTypeName { PrettyFunction.find(TypeName) };
        constexpr std::size_t EndTypeName { BeginTypeName + TypeName.size() };

        return { BeginTypeName, PrettyFunction.size() - EndTypeName };
    }

    [[nodiscard]] consteval std::string_view RemovePrefixes(std::string_view TypeName)
    {
        for (constexpr std::string_view Prefixes[] { "enum ", "union ", "struct ", "class " };
             const auto Prefix : Prefixes)
        {
            if (TypeName.substr(0u, Prefix.size()) == Prefix)
            {
                TypeName.remove_prefix(Prefix.size());
            }
        }

        return { TypeName.data(), TypeName.size() };
    }
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_FUNCTION_NAME_H
