#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H

#include "TypeID.h"
#include "../Hash/DataHash.h"
#include "./Internal/FunctionNameUtils.h"
#include "./Internal/TypeIndex.h"
#include "./Internal/TypeNameUtils.h"

#include <span>
#include <string_view>

namespace egg::Types
{
    template <typename Type>
    struct TypeInfo final
    {
        [[nodiscard]] static consteval std::string_view GetName() noexcept
        {
            constexpr Internal::FunctionBounds Bounds { Internal::GetFunctionBounds() };

            if constexpr (constexpr std::string_view PrettyFunction { Internal::GetFunctionName<Type>() };
                Bounds.BeginTypeName < PrettyFunction.size())
            {
                return Internal::RemovePrefixes(PrettyFunction.substr(
                    Bounds.BeginTypeName,
                    PrettyFunction.size() - Bounds.RestAfterTypeName - Bounds.BeginTypeName
                ));
            }
            else
            {
                return std::string_view {};
            }
        }

        template <typename Group = void, typename SizeType = TypeID>
        [[nodiscard]] static constexpr SizeType GetID() noexcept
        {
            if constexpr (constexpr std::string_view Name { GetName() }; Name.empty())
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
