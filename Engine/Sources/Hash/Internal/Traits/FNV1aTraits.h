#ifndef ENGINE_SOURCES_HASH_TRAITS_FILE_FNV1A_TRAITS_H
#define ENGINE_SOURCES_HASH_TRAITS_FILE_FNV1A_TRAITS_H

#include <cstdint>

namespace egg::Hash::Internal
{
    template <typename>
    struct BasicFNV1aTraits;

    template <>
    struct BasicFNV1aTraits<std::uint32_t>
    {
        using Type = std::uint32_t;
        static constexpr Type Offset { 0x811c9dc5 };
        static constexpr Type Prime { 0x01000193 };
    };

    template <>
    struct BasicFNV1aTraits<std::uint64_t>
    {
        using Type = std::uint64_t;
        static constexpr Type Offset { 0xcbf29ce484222325ull };
        static constexpr Type Prime { 0x100000001b3ull };
    };

    template <typename SizeType>
    class FNV1aTraits : public BasicFNV1aTraits<SizeType>
    {
        using BaseType = BasicFNV1aTraits<SizeType>;

    public:
        using Type = SizeType;

        static constexpr Type Offset { BaseType::Offset };
        static constexpr Type Prime { BaseType::Prime };
    };
}

#endif // ENGINE_SOURCES_HASH_TRAITS_FILE_FNV1A_TRAITS_H
