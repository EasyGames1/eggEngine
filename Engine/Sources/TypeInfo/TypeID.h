#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H

#include "./TypeIndex.h"
#include "./TypeName.h"
#include "../Hash/DataHash.h"

#include <span>
#include <string_view>

namespace egg::TypeInfo
{
    template <typename SizeType, typename Group = void>
    struct TypeID
    {
        template <typename Type>
        [[nodiscard]] static constexpr SizeType Get() noexcept
        {
            if constexpr (constexpr std::string_view Name { TypeName::Get<Type>() }; Name.empty())
            {
                return TypeIndex<SizeType, Group>::template Get<Type>();
            }
            else
            {
                return Hash::DataHash<SizeType> { std::as_bytes(std::span { Name }) }.Murmur();
            }
        }
    };
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H
