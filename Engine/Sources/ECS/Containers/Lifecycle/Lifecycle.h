#ifndef ENGINE_SOURCES_ECS_CONTAINERS_FILE_LIFECYCLE_CONTAINER_H
#define ENGINE_SOURCES_ECS_CONTAINERS_FILE_LIFECYCLE_CONTAINER_H

#include <ECS/Containers/SparseSet/SparseSet.h>
#include <ECS/Containers/Storage/Storage.h>
#include <Events/Signal/Signal.h>
#include <Events/Signal/Sink.h>
#include <Types/Capabilities/Capabilities.h>

#include <iterator>
#include <memory>
#include <type_traits>

namespace egg::ECS
{
    template <ValidEntity EntityType, Types::ValidAllocator<EntityType>>
    class Registry;
}


namespace egg::ECS::Containers
{
    template <typename>
    class Lifecycle;


    template <typename ContainerParameter> requires
        Types::InstanceOf<ContainerParameter, SparseSet> ||
        (Types::InstanceOf<ContainerParameter, Storage> &&
            OptimizableElement<typename ContainerParameter::ElementType, typename ContainerParameter::EntityType>)
    class Lifecycle<ContainerParameter> final
        : public ContainerParameter
    {
        using UnderlyingType = ContainerParameter;
        using EntityParameter = typename UnderlyingType::EntityType;
        using AllocatorParameter = typename UnderlyingType::AllocatorType;

        using OwnerType = Registry<EntityParameter,
                                   typename std::allocator_traits<AllocatorParameter>::template rebind_alloc<EntityParameter>>;
        using SignalType = Events::Signal<void(OwnerType&, EntityParameter), AllocatorParameter>;

    public:
        using ContainerType = UnderlyingType;
        using AllocatorType = typename ContainerType::AllocatorType;

        using EntityType = typename ContainerType::EntityType;
        using Iterator = typename ContainerType::Iterator;

        using RegistryType = OwnerType;
        using SinkType = Events::Sink<SignalType>;


        constexpr explicit Lifecycle(RegistryType& Owner, const AllocatorType& Allocator = {})
            noexcept(
                std::is_nothrow_constructible_v<ContainerType, const AllocatorType&> &&
                std::is_nothrow_constructible_v<SignalType, const AllocatorType&>)
            : ContainerType { Allocator },
              Owner { Owner },
              Construction { Allocator },
              Destruction { Allocator }
        {
        }

        Lifecycle(const Lifecycle&) = delete;

        constexpr Lifecycle(Lifecycle&& Other)
            noexcept(std::is_nothrow_move_constructible_v<ContainerType> && std::is_nothrow_move_constructible_v<SignalType>) = default;

        constexpr Lifecycle(Lifecycle&& Other, const AllocatorType& Allocator)
            : ContainerType { std::move(Other), Allocator },
              Owner { Other.Owner },
              Construction { std::move(Other.Construction), Allocator },
              Destruction { std::move(Other.Destruction), Allocator }
        {
        }

        constexpr ~Lifecycle() noexcept override = default;

        Lifecycle& operator=(const Lifecycle&) = delete;

        constexpr Lifecycle& operator=(Lifecycle&& Other)
            noexcept(std::is_nothrow_move_assignable_v<ContainerType> && std::is_nothrow_move_assignable_v<SignalType>) = default;

        constexpr friend void swap(Lifecycle& Left, Lifecycle& Right)
            noexcept(std::is_nothrow_swappable_v<ContainerType> && std::is_nothrow_swappable_v<SignalType>)
        {
            using std::swap;
            swap(static_cast<ContainerType&>(Left), static_cast<ContainerType&>(Right));
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
                for (std::size_t Position = 0u, Last = ContainerType::GetSize(); Position < Last; ++Position)
                {
                    Destruction.Publish(Owner, ContainerType::operator[](Position));
                }
            }

            ContainerType::Clear();
        }

    protected:
        constexpr Iterator TryEmplace(const EntityType Entity) override
        {
            const Iterator Constructed { ContainerType::TryEmplace(Entity) };
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

            ContainerType::Pop(First, Last);
        }

    private:
        OwnerType& Owner;
        SignalType Construction;
        SignalType Destruction;
    };

    template <Types::InstanceOf<Storage> ContainerParameter>
        requires (!OptimizableElement<typename ContainerParameter::ElementType, typename ContainerParameter::EntityType>)
    class Lifecycle<ContainerParameter> final
        : public ContainerParameter
    {
        using UnderlyingType = ContainerParameter;
        using EntityParameter = typename UnderlyingType::EntityType;

        using SparseSetType = typename UnderlyingType::BaseType;
        using SparseSetIterator = typename SparseSetType::Iterator;

        using OwnerType = Registry<EntityParameter, typename SparseSetType::AllocatorType>;
        using SignalType = Events::Signal<void(OwnerType&, EntityParameter), typename UnderlyingType::AllocatorType>;

    public:
        using ContainerType = UnderlyingType;
        using AllocatorType = typename ContainerType::AllocatorType;

        using EntityType = typename ContainerType::EntityType;
        using ElementType = typename ContainerType::ElementType;

        using ConstReference = typename ContainerType::ConstReference;
        using Reference = typename ContainerType::Reference;

        using Iterator = typename ContainerType::Iterator;

        using RegistryType = OwnerType;
        using SinkType = Events::Sink<SignalType>;


        constexpr explicit Lifecycle(RegistryType& Owner, const AllocatorType& Allocator = {})
            noexcept(
                std::is_nothrow_constructible_v<ContainerType, const AllocatorType&> &&
                std::is_nothrow_constructible_v<SignalType, const AllocatorType&>)
            : ContainerType { Allocator },
              Owner { Owner },
              Construction { Allocator },
              Update { Allocator },
              Destruction { Allocator }
        {
        }

        Lifecycle(const Lifecycle&) = delete;

        constexpr Lifecycle(Lifecycle&& Other)
            noexcept(std::is_nothrow_move_constructible_v<ContainerType> && std::is_nothrow_move_constructible_v<SignalType>) = default;

        constexpr Lifecycle(Lifecycle&& Other, const AllocatorType& Allocator)
            : ContainerType { std::move(Other), Allocator },
              Owner { Other.Owner },
              Construction { std::move(Other.Construction), Allocator },
              Update { std::move(Other.Update), Allocator },
              Destruction { std::move(Other.Destruction), Allocator }
        {
        }

        constexpr ~Lifecycle() noexcept override = default;

        Lifecycle& operator=(const Lifecycle&) = delete;

        constexpr Lifecycle& operator=(Lifecycle&& Other)
            noexcept(std::is_nothrow_move_assignable_v<ContainerType> && std::is_nothrow_move_assignable_v<SignalType>) = default;

        constexpr friend void swap(Lifecycle& Left, Lifecycle& Right)
            noexcept(std::is_nothrow_swappable_v<ContainerType> && std::is_nothrow_swappable_v<SignalType>)
        {
            using std::swap;
            swap(static_cast<ContainerType&>(Left), static_cast<ContainerType&>(Right));
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
            ElementType& Constructed { ContainerType::Emplace(Entity, std::forward<Args>(Arguments)...) };
            Construction.Publish(Owner, Entity);
            return Constructed;
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType, typename... Args>
        constexpr Iterator Insert(IteratorType First, SentinelType Last, Args&&... Arguments)
        {
            std::size_t From { ContainerType::GetSize() };
            const Iterator Constructed { ContainerType::Insert(First, Last, std::forward<Args>(Arguments)...) };

            if (!Construction.Empty())
            {
                for (const std::size_t To { ContainerType::GetSize() }; From < To; ++From)
                {
                    Construction.Publish(Owner, ContainerType::operator[](From));
                }
            }

            return Constructed;
        }

        template <typename... CallableTypes>
        constexpr ElementType& Patch(const EntityType Entity, CallableTypes&&... Callables)
        {
            ElementType& Updated { ContainerType::Patch(Entity, std::forward<CallableTypes>(Callables)...) };
            Update.Publish(Owner, Entity);
            return Updated;
        }

        constexpr void Clear() override
        {
            if (!Destruction.Empty())
            {
                for (std::size_t Position = 0u, Last = ContainerType::GetSize(); Position < Last; ++Position)
                {
                    Destruction.Publish(Owner, ContainerType::operator[](Position));
                }
            }

            ContainerType::Clear();
        }

    protected:
        constexpr SparseSetIterator TryEmplace(const EntityType Entity) override
        {
            const SparseSetIterator Constructed { ContainerType::TryEmplace(Entity) };

            if constexpr (std::default_initializable<typename ContainerType::ElementType>)
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

            ContainerType::Pop(First, Last);
        }

    private:
        OwnerType& Owner;
        SignalType Construction;
        SignalType Update;
        SignalType Destruction;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_FILE_LIFECYCLE_CONTAINER_H
