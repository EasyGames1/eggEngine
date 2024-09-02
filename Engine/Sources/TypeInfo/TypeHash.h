#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_HASH_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_HASH_H

#include "TypeID.h"
#include "TypeIndex.h"
#include "TypeInfo.h"
#include "../Config/Config.h"

#include <string_view>


namespace egg::TypeInfo
{
    template <typename Group = void>
    struct TypeHash final
    {
        template <typename Type>
        [[nodiscard]] static constexpr TypeID Get() noexcept
        {
            if constexpr (IsPrettyFunction)
            {
                return GetTypeID(GetTypeName<Type>());
            }
            else
            {
                return TypeIndex<Group>::template Get<Type>();
            }
        }
    };
}


#endif // ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_HASH_H
