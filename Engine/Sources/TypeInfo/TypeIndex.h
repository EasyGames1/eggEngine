#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H

#include "IDType.h"

namespace egg::TypeInfo
{
    template <typename = void>
    class TypeIndex final
    {
    public:
        template <typename>
        [[nodiscard]] constexpr static IDType Get() noexcept
        {
            static const IDType Index { Next() };
            return Index;
        }

    private:
        [[nodiscard]] constexpr static IDType Next() noexcept
        {
            static std::size_t Counter {};
            return Counter++;
        }
    };
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H
