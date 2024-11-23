#ifndef ENGINE_SOURCES_ECS_CONTAINERS_FILE_LIFECYCLE_CONTAINER_H
#define ENGINE_SOURCES_ECS_CONTAINERS_FILE_LIFECYCLE_CONTAINER_H

#include "Events/Signal.h"
#include "Events/Sink.h"
#include "SparseSet/SparseSet.h"
#include "Storage/Storage.h"
#include "Types/Traits/Capabilities.h"

namespace egg::ECS
{
    template <ValidEntity EntityType, Types::ValidAllocator<EntityType>>
    class Registry;
}


namespace egg::ECS::Containers
{
    template <typename>
    class LifecycleContainer;


    template <typename ContainerType> requires
        Types::InstanceOf<ContainerType, SparseSet> ||
        (Types::InstanceOf<ContainerType, Storage> &&
            OptimizableElement<typename ContainerType::ElementType, typename ContainerType::EntityType>)
    class LifecycleContainer<ContainerType> final
        : public ContainerType
    {
        using EntityParameter = typename ContainerType::EntityType;
        using AllocatorParameter = typename ContainerType::AllocatorType;
        using EntityAllocatorType = typename std::allocator_traits<AllocatorParameter>::template rebind_alloc<EntityParameter>;

        using BaseType = ContainerType;
        using OwnerType = Registry<EntityParameter, EntityAllocatorType>;

        using SignalType = Events::Signal<void(OwnerType&, EntityParameter), AllocatorParameter>;

    public:
        using AllocatorType = typename BaseType::AllocatorType;

        using EntityType = typename BaseType::EntityType;
        using Iterator = typename BaseType::Iterator;

        using RegistryType = OwnerType;
        using SinkType = Events::Sink<SignalType>;


        constexpr explicit LifecycleContainer(RegistryType& Owner, const AllocatorType& Allocator = {})
            noexcept(
                std::is_nothrow_constructible_v<BaseType, const AllocatorType&> &&
                std::is_nothrow_constructible_v<SignalType, const AllocatorType&>)
            : BaseType { Allocator },
              Owner { Owner },
              Construction { Allocator },
              Destruction { Allocator }
        {
        }

        LifecycleContainer(const LifecycleContainer&) = delete;

        constexpr LifecycleContainer(LifecycleContainer&& Other)
            noexcept(std::is_nothrow_move_constructible_v<BaseType> && std::is_nothrow_move_constructible_v<SignalType>) = default;

        constexpr LifecycleContainer(LifecycleContainer&& Other, const AllocatorType& Allocator)
            : BaseType { std::move(Other), Allocator },
              Owner { Other.Owner },
              Construction { std::move(Other.Construction), Allocator },
              Destruction { std::move(Other.Destruction), Allocator }
        {
        }

        constexpr ~LifecycleContainer() noexcept override = default;

        LifecycleContainer& operator=(const LifecycleContainer&) = delete;

        constexpr LifecycleContainer& operator=(LifecycleContainer&& Other)
            noexcept(std::is_nothrow_move_assignable_v<BaseType> && std::is_nothrow_move_assignable_v<SignalType>) = default;

        constexpr friend void swap(LifecycleContainer& Left, LifecycleContainer& Right)
            noexcept(std::is_nothrow_swappable_v<BaseType> && std::is_nothrow_swappable_v<SignalType>)
        {
            using std::swap;
            swap(static_cast<BaseType&>(Left), static_cast<BaseType&>(Right));
            swap(Left.Owner, Right.Owner);
            swap(Left.Construction, Right.Construction);
            swap(Left.Destruction, Right.Destruction);
        }

        [[nodiscard]] constexpr SinkType OnConstruct() noexcept
        {
            return SinkType { Construction };
        }

        [[nodiscard]] constexpr SinkType OnDestroy() noexcept
        {
            return SinkType { Destruction };
        }

        constexpr void Clear() override
        {
            if (!Destruction.Empty())
            {
                for (std::size_t Position = 0u, Last = BaseType::GetSize(); Position < Last; ++Position)
                {
                    Destruction.Publish(Owner, BaseType::operator[](Position));
                }
            }

            BaseType::Clear();
        }

    protected:
        constexpr Iterator TryEmplace(const EntityType Entity) override
        {
            const Iterator Constructed { BaseType::TryEmplace(Entity) };
            Construction.Publish(Owner, *Constructed);
            return Constructed;
        }

        constexpr void Pop(Iterator First, Iterator Last) override
        {
            if (!Destruction.Empty())
            {
                for (Iterator Copy { First }; Copy != Last; ++Copy)
                {
                    Destruction.Publish(Owner, *Copy);
                }
            }

            BaseType::Pop(First, Last);
        }

    private:
        OwnerType& Owner;
        SignalType Construction;
        SignalType Destruction;
    };

    template <Types::InstanceOf<Storage> ContainerType>
        requires (!OptimizableElement<typename ContainerType::ElementType, typename ContainerType::EntityType>)
    class LifecycleContainer<ContainerType> final
        : public Storage<typename ContainerType::ElementType, typename ContainerType::EntityType, typename ContainerType::AllocatorType>
    {
        using ElementParameter = typename ContainerType::ElementType;
        using EntityParameter = typename ContainerType::EntityType;
        using AllocatorParameter = typename ContainerType::AllocatorType;


        using UnderlyingType = Storage<ElementParameter, EntityParameter, AllocatorParameter>;
        using SparseSetType = typename UnderlyingType::BaseType;
        using SparseSetIterator = typename SparseSetType::Iterator;

        using OwnerType = Registry<EntityParameter, typename SparseSetType::AllocatorType>;
        using SignalType = Events::Signal<void(OwnerType&, EntityParameter), AllocatorParameter>;

    public:
        using AllocatorType = typename UnderlyingType::AllocatorType;

        using EntityType = typename UnderlyingType::EntityType;
        using ElementType = typename UnderlyingType::ElementType;

        using ConstReference = typename UnderlyingType::ConstReference;
        using Reference = typename UnderlyingType::Reference;

        using Iterator = typename UnderlyingType::Iterator;

        using RegistryType = OwnerType;
        using SinkType = Events::Sink<SignalType>;


        constexpr explicit LifecycleContainer(RegistryType& Owner, const AllocatorType& Allocator = {})
            noexcept(
                std::is_nothrow_constructible_v<UnderlyingType, const AllocatorType&> &&
                std::is_nothrow_constructible_v<SignalType, const AllocatorType&>)
            : UnderlyingType { Allocator },
              Owner { Owner },
              Construction { Allocator },
              Update { Allocator },
              Destruction { Allocator }
        {
        }

        LifecycleContainer(const LifecycleContainer&) = delete;

        constexpr LifecycleContainer(LifecycleContainer&& Other)
            noexcept(std::is_nothrow_move_constructible_v<UnderlyingType> && std::is_nothrow_move_constructible_v<SignalType>) = default;

        constexpr LifecycleContainer(LifecycleContainer&& Other, const AllocatorType& Allocator)
            : UnderlyingType { std::move(Other), Allocator },
              Owner { Other.Owner },
              Construction { std::move(Other.Construction), Allocator },
              Update { std::move(Other.Update), Allocator },
              Destruction { std::move(Other.Destruction), Allocator }
        {
        }

        constexpr ~LifecycleContainer() noexcept override = default;

        LifecycleContainer& operator=(const LifecycleContainer&) = delete;

        constexpr LifecycleContainer& operator=(LifecycleContainer&& Other)
            noexcept(std::is_nothrow_move_assignable_v<UnderlyingType> && std::is_nothrow_move_assignable_v<SignalType>) = default;

        constexpr friend void swap(LifecycleContainer& Left, LifecycleContainer& Right)
            noexcept(std::is_nothrow_swappable_v<UnderlyingType> && std::is_nothrow_swappable_v<SignalType>)
        {
            using std::swap;
            swap(static_cast<UnderlyingType&>(Left), static_cast<UnderlyingType&>(Right));
            swap(Left.Owner, Right.Owner);
            swap(Left.Construction, Right.Construction);
            swap(Left.Update, Right.Update);
            swap(Left.Destruction, Right.Destruction);
        }

        [[nodiscard]] constexpr SinkType OnConstruct() noexcept
        {
            return SinkType { Construction };
        }

        [[nodiscard]] constexpr SinkType OnUpdate() noexcept
        {
            return SinkType { Update };
        }

        [[nodiscard]] constexpr SinkType OnDestroy() noexcept
        {
            return SinkType { Destruction };
        }

        template <typename... Args>
        constexpr ElementType& Emplace(const EntityType Entity, Args&&... Arguments)
        {
            ElementType& Constructed { UnderlyingType::Emplace(Entity, std::forward<Args>(Arguments)...) };
            Construction.Publish(Owner, Entity);
            return Constructed;
        }

        template <typename IteratorType, typename... Args>
        constexpr Iterator Insert(IteratorType First, IteratorType Last, Args&&... Arguments)
        {
            std::size_t From { UnderlyingType::GetSize() };
            const Iterator Constructed { UnderlyingType::Insert(First, Last, std::forward<Args>(Arguments)...) };

            if (!Construction.Empty())
            {
                for (const std::size_t To { UnderlyingType::GetSize() }; From < To; ++From)
                {
                    Construction.Publish(Owner, UnderlyingType::operator[](From));
                }
            }

            return Constructed;
        }

        template <typename... Func>
        constexpr ElementType& Patch(const EntityType Entity, Func&&... Functions)
        {
            ElementType& Updated { UnderlyingType::Patch(Entity, std::forward<Func>(Functions)...) };
            Update.Publish(Owner, Entity);
            return Updated;
        }

        constexpr void Clear() override
        {
            if (!Destruction.Empty())
            {
                for (std::size_t Position = 0u, Last = UnderlyingType::GetSize(); Position < Last; ++Position)
                {
                    Destruction.Publish(Owner, UnderlyingType::operator[](Position));
                }
            }

            UnderlyingType::Clear();
        }

    protected:
        constexpr SparseSetIterator TryEmplace(const EntityType Entity) override
        {
            const SparseSetIterator Constructed { UnderlyingType::TryEmplace(Entity) };

            if constexpr (std::default_initializable<typename UnderlyingType::ElementType>)
            {
                Construction.Publish(Owner, *Constructed);
            }

            return Constructed;
        }

        constexpr void Pop(SparseSetIterator First, SparseSetIterator Last) override
        {
            if (!Destruction.Empty())
            {
                for (SparseSetIterator Copy { First }; Copy != Last; ++Copy)
                {
                    Destruction.Publish(Owner, *Copy);
                }
            }

            UnderlyingType::Pop(First, Last);
        }

    private:
        OwnerType& Owner;
        SignalType Construction;
        SignalType Update;
        SignalType Destruction;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_FILE_LIFECYCLE_CONTAINER_H
