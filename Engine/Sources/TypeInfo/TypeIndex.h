#ifndef ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H
#define ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H

namespace egg::Types::Internal
{
    template <typename SizeType, typename = void>
    class TypeIndex final
    {
    public:
        using IndexType = SizeType;

        template <typename>
        [[nodiscard]] static IndexType Get() noexcept
        {
            static const IndexType Index { Counter++ };
            return Index;
        }

    private:
        static IndexType Counter;
    };

    template <typename SizeType, typename Group>
    typename TypeIndex<SizeType, Group>::IndexType TypeIndex<SizeType, Group>::Counter {};
}

#endif // ENGINE_SOURCES_TYPE_INFO_FILE_TYPE_INDEX_H
