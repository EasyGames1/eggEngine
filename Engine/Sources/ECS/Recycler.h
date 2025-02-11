#ifndef ENGINE_SOURCES_ECS_CONTAINERS_RECYCLER_FILE_RECYCLER_H
#define ENGINE_SOURCES_ECS_CONTAINERS_RECYCLER_FILE_RECYCLER_H

#include <ECS/Entity.h>
#include <ECS/Containers/SparseSet/SparseSet.h>
#include <Types/Capabilities/Capabilities.h>

#include <memory>

namespace egg::ECS
{
    template <ValidEntity EntityParameter, Types::ValidAllocator<EntityParameter> AllocatorParameter = std::allocator<EntityParameter>>
    class Recycler final
    {
        using ContainerType = Containers::SparseSet<EntityParameter, AllocatorParameter>;

        using TraitsType = EntityTraits<EntityParameter>;

    public:
        using AllocatorType = AllocatorParameter;

        using EntityType = typename TraitsType::ValueType;
        using VersionType = typename TraitsType::VersionType;

        using Iterator = typename ContainerType::Iterator;
        using ReverseIterator = typename ContainerType::ReverseIterator;


        constexpr explicit Recycler(const AllocatorType& Allocator = {})
            noexcept(std::is_nothrow_constructible_v<ContainerType, const AllocatorType&>)
            : Entities { Allocator },
              ValidCount {}
        {
        }

        Recycler(const Recycler&) = delete;

        constexpr Recycler(Recycler&&) noexcept(std::is_nothrow_move_constructible_v<ContainerType>) = default;

        constexpr Recycler(Recycler&& Other, const AllocatorType& Allocator)
            : Entities { std::move(Other.Entities), Allocator },
              ValidCount { Other.ValidCount }
        {
        }

        constexpr ~Recycler() noexcept = default;

        Recycler& operator=(const Recycler&) = delete;

        constexpr Recycler& operator=(Recycler&&) noexcept(std::is_nothrow_move_assignable_v<ContainerType>) = default;

        constexpr friend void swap(Recycler& Left, Recycler& Right) noexcept (std::is_nothrow_swappable_v<ContainerType>)
        {
            using std::swap;
            swap(Left.Entities, Right.Entities);
            swap(Left.ValidCount, Right.ValidCount);
        }

        [[nodiscard]] constexpr EntityType Create()
        {
            const EntityType Created { ValidCount == Entities.GetSize() ? CreateNew() : GetRecycled() };
            ++ValidCount;
            return Created;
        }

        [[nodiscard]] constexpr bool Valid(const EntityType Entity) const noexcept
        {
            return Entities.Contains(Entity) && Entities.GetIndex(Entity) < ValidCount;
        }

        [[nodiscard]] constexpr VersionType GetVersion(const EntityType Entity) const noexcept
        {
            //todo Why it returns version even if Entity is destroyed?
            return Entities.GetVersion(Entity);
        }

        constexpr VersionType Recycle(const EntityType Entity)
        {
            EGG_ASSERT(Valid(Entity), "Invalid entity");
            Entities.SwapElementsAt(Entities.GetIndex(Entity), --ValidCount);
            return SetNextVersion(Entity);
        }

        constexpr void Recycle()
        {
            for (std::size_t Position = ValidCount; Position--;)
            {
                SetNextVersion(Entities[Position]);
            }

            ValidCount = 0u;
        }

        [[nodiscard]] constexpr AllocatorType GetAllocator() const noexcept
        {
            return Entities.GetAllocator();
        }

    private:
        [[nodiscard]] constexpr EntityType CreateNew()
        {
            return *Entities.Push(GetNextEntity());
        }

        [[nodiscard]] constexpr EntityType GetNextEntity() const noexcept
        {
            return TraitsType::Construct(Entities.GetSize(), {});
        }

        [[nodiscard]] constexpr EntityType GetRecycled() const noexcept
        {
            return Entities[ValidCount];
        }

        constexpr VersionType SetNextVersion(const EntityType Entity)
        {
            return Entities.UpdateVersion(TraitsType::GetNext(Entity));
        }

        ContainerType Entities;
        std::size_t ValidCount;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_RECYCLER_FILE_RECYCLER_H
