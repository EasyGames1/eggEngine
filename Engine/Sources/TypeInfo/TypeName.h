#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_NAME_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_NAME_H

#include "FunctionName.h"

#include <source_location>
#include <string_view>

namespace egg::TypeInfo
{
    class TypeName
    {
    public:
        template <typename Type>
        [[nodiscard]] static consteval std::string_view Get() noexcept
        {
            const auto [Begin, Rest] { Internal::FunctionName::GetBounds() };

            if (Begin == std::string_view::npos)
            {
                return std::string_view {};
            }

            constexpr std::string_view PrettyFunction { Internal::FunctionName::Get<Type>() };

            return RemovePrefixes(PrettyFunction.substr(Begin, PrettyFunction.size() - Rest - Begin));
        }

    private:
        [[nodiscard]] static consteval std::string_view RemovePrefixes(std::string_view TypeName) noexcept
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
    };
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_NAME_H
