#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H

#include "IDType.h"
#include "TypeIndex.h"
#include "../Hash/HashString.h"
#include "TypeName.h"

#include <string_view>

namespace egg::TypeInfo
{
    template <typename Group = void>
    struct TypeID
    {
        template <typename Type>
        [[nodiscard]] static constexpr IDType Get() noexcept
        {
            if constexpr (constexpr std::string_view Name { TypeName::Get<Type>() }; Name.empty())
            {
                return TypeIndex<Group>::template Get<Type>();
            }
            else
            {
                return Hash::HashString { Name }.GetMurmur<IDType>();
            }
        }
    };
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_TYPEID_H
