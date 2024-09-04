#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_FUNCTION_NAME_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_FUNCTION_NAME_H

#include <source_location>
#include <string_view>

namespace egg::TypeInfo::Internal
{
    struct FunctionName
    {
        template <typename>
        [[nodiscard]] static consteval std::string_view Get() noexcept
        {
            return std::source_location::current().function_name();
        }

        [[nodiscard]] static consteval std::pair<std::size_t, std::size_t> GetBounds() noexcept
        {
            constexpr std::string_view PrettyFunction { Get<long double>() };

            constexpr std::string_view TypeName { "long double" };
            constexpr std::size_t BeginTypeName { PrettyFunction.find(TypeName) };
            constexpr std::size_t EndTypeName { BeginTypeName + TypeName.size() };

            return { BeginTypeName, PrettyFunction.size() - EndTypeName };
        }
    };
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_FUNCTION_NAME_H
