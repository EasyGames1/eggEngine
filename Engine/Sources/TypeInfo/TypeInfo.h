#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H

#include "./TypeIndex.h"
#include "../Hash/DataHash.h"
#include "Internal.h"

#include <span>
#include <string_view>

namespace egg::Types
{
    template <typename SizeType, typename Group = void>
    struct TypeInfo
    {
        template <typename Type>
        [[nodiscard]] static consteval std::string_view GetName() noexcept
        {
            const auto [Begin, Rest] { Internal::GetFunctionBounds() };

            if (Begin == std::string_view::npos)
            {
                return std::string_view {};
            }

            constexpr std::string_view PrettyFunction { Internal::GetFunctionName<Type>() };

            return Internal::RemovePrefixes(PrettyFunction.substr(Begin, PrettyFunction.size() - Rest - Begin));
        }

        template <typename Type>
        [[nodiscard]] static constexpr SizeType GetID() noexcept
        {
            if constexpr (constexpr std::string_view Name { GetName<Type>() }; Name.empty())
            {
                return Internal::TypeIndex<SizeType, Group>::template Get<Type>();
            }
            else
            {
                return Hash::DataHash<SizeType> { std::as_bytes(std::span { Name }) }.Murmur2();
            }
        }
    };
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H