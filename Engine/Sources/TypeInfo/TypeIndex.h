#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H

#include "TypeID.h"

namespace egg::TypeInfo
{
    template <typename = void>
    class TypeIndex final
    {
    public:
        template <typename>
        [[nodiscard]] static TypeID Get() noexcept
        {
            static const TypeID Index { Next() };
            return Index;
        }

    private:
        [[nodiscard]] static TypeID Next() noexcept
        {
            static std::size_t Counter {};
            return Counter++;
        }
    };
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H
