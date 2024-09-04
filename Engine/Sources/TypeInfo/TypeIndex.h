#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H

namespace egg::TypeInfo
{
    template <typename SizeType, typename = void>
    class TypeIndex final
    {
    public:
        template <typename>
        [[nodiscard]] constexpr static SizeType Get() noexcept
        {
            static const SizeType Index { Next() };
            return Index;
        }

    private:
        [[nodiscard]] constexpr static SizeType Next() noexcept
        {
            static SizeType Counter {};
            return Counter++;
        }
    };
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H
