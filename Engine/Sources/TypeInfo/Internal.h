#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_FUNCTION_NAME_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_FUNCTION_NAME_H

#include <source_location>
#include <string_view>

namespace egg::TypeInfo::Internal
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

    [[nodiscard]] consteval std::string_view RemovePrefixes(std::string_view TypeName) noexcept
    {
        {
            for (constexpr std::string_view Prefixes[] { "struct ", "class ", "enum ", "union " };
                 const auto Prefix : Prefixes)
            {
                for (std::size_t Position = TypeName.find(Prefix); Position != std::string_view::npos; Position = TypeName.find(Prefix))
                {
                    TypeName.remove_prefix(Position + Prefix.size());
                }
            }

            return { TypeName.data(), TypeName.size() };
        }
    }
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_FUNCTION_NAME_H
