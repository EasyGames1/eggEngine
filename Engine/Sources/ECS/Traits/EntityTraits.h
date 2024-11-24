#ifndef ENGINE_SOURCES_ECS_TRAITS_FILE_ENTITY_TRAITS_H
#define ENGINE_SOURCES_ECS_TRAITS_FILE_ENTITY_TRAITS_H

#include <ECS/Traits/BasicEntityTraits.h>
#include <ECS/Traits/PageSizeTraits.h>

#include <bit>

namespace egg::ECS
{
    template <typename Type>
    class EntityTraits : BasicEntityTraits<Type>
    {
        using BaseType = BasicEntityTraits<Type>;

        static constexpr std::size_t Length { std::popcount(BaseType::EntityMask) };

        static_assert(!(BaseType::EntityMask && BaseType::EntityMask & BaseType::EntityMask + 1u), "Invalid entity mask");
        static_assert(!(BaseType::VersionMask & BaseType::VersionMask + 1u), "Invalid version mask");

    public:
        using ValueType = typename BaseType::ValueType;

        using IntegralType = typename BaseType::IntegralType;
        using EntityType = typename BaseType::EntityType;
        using VersionType = typename BaseType::VersionType;

        static constexpr EntityType EntityMask { BaseType::EntityMask };
        static constexpr VersionType VersionMask { BaseType::VersionMask };

        [[nodiscard]] static constexpr IntegralType ToIntegral(const ValueType Value) noexcept
        {
            return static_cast<IntegralType>(Value);
        }

        [[nodiscard]] static constexpr EntityType ToEntity(const ValueType Value) noexcept
        {
            return ToIntegral(Value) & EntityMask;
        }

        [[nodiscard]] static constexpr IntegralType ToVersionPart(const ValueType Value) noexcept
        {
            if constexpr (!VersionMask)
            {
                return IntegralType {};
            }
            else
            {
                constexpr auto VersionPartMask { ToIntegral(Tombstone) & ~EntityMask };
                return ToIntegral(Value) & VersionPartMask;
            }
        }

        [[nodiscard]] static constexpr VersionType ToVersion(const ValueType Value) noexcept
        {
            if constexpr (!VersionMask)
            {
                return VersionType {};
            }
            else
            {
                return static_cast<VersionType>(ToIntegral(Value) >> Length);
            }
        }

        [[nodiscard]] static constexpr ValueType GetNext(const ValueType Value) noexcept
        {
            const auto Version { ToVersion(Value) + 1u };
            return Construct(ToIntegral(Value), static_cast<VersionType>(Version + (Version == VersionMask)));
        }

        [[nodiscard]] static constexpr ValueType Construct(const EntityType Entity, const VersionType Version) noexcept
        {
            if constexpr (!VersionMask)
            {
                return ValueType { Entity & EntityMask };
            }
            else
            {
                return ValueType { Entity & EntityMask | static_cast<IntegralType>(Version & VersionMask) << Length };
            }
        }

        [[nodiscard]] static constexpr ValueType Combine(const EntityType Left, const IntegralType Right) noexcept
        {
            if constexpr (!VersionMask)
            {
                return ValueType { Left & EntityMask };
            }
            else
            {
                return ValueType { Left & EntityMask | Right & VersionMask << Length };
            }
        }

        static constexpr EntityType PageSize { PageSizeTraits<ValueType>::value };
        static_assert(std::same_as<typename PageSizeTraits<ValueType>::value_type, EntityType>, "Page size type must be entity type");
        static_assert(std::has_single_bit(PageSize), "Page size must be a power of two");

        static constexpr ValueType Tombstone { Construct(EntityMask, VersionMask) };
    };
}

#endif // ENGINE_SOURCES_ECS_TRAITS_FILE_ENTITY_TRAITS_H
