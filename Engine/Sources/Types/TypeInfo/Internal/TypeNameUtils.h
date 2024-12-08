#ifndef ENGINE_SOURCES_TYPE_INFO_INTERNAL_FILE_TYPE_NAME_UTILS_H
#define ENGINE_SOURCES_TYPE_INFO_INTERNAL_FILE_TYPE_NAME_UTILS_H

#include <string_view>

namespace egg::Types::Internal
{
    [[nodiscard]] constexpr std::string_view RemovePrefixes(std::string_view TypeName)
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

#endif // ENGINE_SOURCES_TYPE_INFO_INTERNAL_FILE_TYPE_NAME_UTILS_H
