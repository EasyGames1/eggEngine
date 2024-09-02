#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INFO_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INFO_H

#include "TypeID.h"
#include "../Config/Config.h"
#include "../Hash/HashString.h"

#include <string_view>

namespace egg::TypeInfo
{
    template <typename>
    [[nodiscard]] constexpr std::string_view GetTypeName() noexcept
    {
        constexpr std::string_view PrettyFunctionName { EGG_PRETTY_FUNCTION };
        constexpr std::size_t PrefixIndex { PrettyFunctionName.find(PrettyFunctionPrefix) + 1u };
        constexpr std::size_t StartIndex { PrettyFunctionName.find(' ', PrefixIndex) + 1u };
        constexpr std::size_t EndIndex { PrettyFunctionName.find_first_of(PrettyFunctionSuffix) };
        return PrettyFunctionName.substr(StartIndex, EndIndex - StartIndex);
    }

    [[nodiscard]] constexpr TypeID GetTypeID(const std::string_view TypeName) noexcept
    {
        return Hash::HashString { TypeName }.GetMurmur<TypeID>();
    }
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INFO_H
