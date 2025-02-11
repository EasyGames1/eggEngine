#ifndef ENGINE_SOURCES_ECS_CONTAINERS_SPARSE_SET_FILE_SPARSE_SET_H
#define ENGINE_SOURCES_ECS_CONTAINERS_SPARSE_SET_FILE_SPARSE_SET_H

#include "./Internal/SparseSetIterator.h"

#include <Containers/PagedVector/PagedVector.h>
#include <ECS/Entity.h>
#include <ECS/Containers/Container.h>
#include <ECS/Traits/EntityTraits.h>
#include <Types/Capabilities/Capabilities.h>

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <vector>

namespace egg::ECS::Containers
{
    template <ValidEntity EntityParameter, Types::ValidAllocator<EntityParameter> AllocatorParameter = std::allocator<EntityParameter>>
    class SparseSet
    {
        using ContainerAllocatorTraits = AllocatorTraits<AllocatorParameter>;

        using SparseContainer = PagedVector<EntityParameter, AllocatorParameter>;
        using PackedContainer = std::vector<EntityParameter, AllocatorParameter>;

        using TraitsType = EntityTraits<EntityParameter>;

        using Reference = typename PackedContainer::reference;

    public:
        using AllocatorType = AllocatorParameter;
        using ConstPointer = typename PackedContainer::const_pointer;

        using EntityType = typename TraitsType::ValueType;
        using VersionType = typename TraitsType::VersionType;

        using Iterator = Internal::SparseSetIterator<PackedContainer>;
        using ConstIterator = Iterator;
        using ReverseIterator = std::reverse_iterator<Iterator>;
        using ConstReverseIterator = std::reverse_iterator<ConstIterator>;


        constexpr explicit SparseSet(const AllocatorType& Allocator = {})
            noexcept(std::is_nothrow_constructible_v<SparseContainer, const AllocatorType&>)
            : Sparse { Allocator }, Packed { Allocator }
        {
        }

        SparseSet(const SparseSet&) = delete;

        constexpr SparseSet(SparseSet&&) noexcept(std::is_nothrow_move_constructible_v<SparseContainer>) = default;

        constexpr SparseSet(SparseSet&& Other, const AllocatorType& Allocator) : Sparse { std::move(Other.Sparse), Allocator },
                                                                                 Packed { std::move(Other.Packed), Allocator }
        {
            EGG_ASSERT(ContainerAllocatorTraits::is_always_equal::value || GetAllocator() == Other.GetAllocator(),
                       "Cannot move sparse set because it has an incompatible allocator");
        }

        constexpr virtual ~SparseSet() noexcept = default;

        SparseSet& operator=(const SparseSet&) = delete;

        constexpr SparseSet& operator=(SparseSet&& Other) noexcept(std::is_nothrow_move_assignable_v<SparseContainer>)
        {
            EGG_ASSERT(ContainerAllocatorTraits::is_always_equal::value || GetAllocator() == Other.GetAllocator(),
                       "Cannot move sparse set because it has an incompatible allocator");
            Sparse = std::move(Other.Sparse);
            Packed = std::move(Other.Packed);
            return *this;
        }

        constexpr friend void swap(SparseSet& Left, SparseSet& Right)
            noexcept(std::is_nothrow_swappable_v<SparseContainer> && std::is_nothrow_swappable_v<PackedContainer>)
        {
            using std::swap;
            swap(Left.Sparse, Right.Sparse);
            swap(Left.Packed, Right.Packed);
        }

        constexpr Iterator Push(const EntityType Entity)
        {
            return TryEmplace(Entity);
        }

        constexpr VersionType UpdateVersion(const EntityType Entity)
        {
            EGG_ASSERT(Entity != TraitsType::Tombstone, "Invalid entity");
            auto& Element { GetReference(Entity) };
            EGG_ASSERT(Element != TraitsType::Tombstone, "Set does not contain entity");
            Element = TraitsType::Combine(TraitsType::ToIntegral(Element), TraitsType::ToIntegral(Entity));
            Packed[static_cast<std::size_t>(TraitsType::ToEntity(Element))] = Entity;
            return TraitsType::ToVersion(Entity);
        }

        constexpr void Erase(const EntityType Entity)
        {
            const auto It { ToIterator(Entity) };
            Pop(It, It + 1);
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr void Erase(IteratorType First, SentinelType Last)
        {
            if constexpr (std::same_as<IteratorType, Iterator>)
            {
                Pop(First, Last);
            }
            else
            {
                for (; First != Last; ++First)
                {
                    Erase(*First);
                }
            }
        }

        constexpr virtual void Clear()
        {
            for (auto Entity : *this)
            {
                GetReference(Entity) = TraitsType::Tombstone;
            }
            Packed.clear();
        }

        constexpr bool Remove(const EntityType Entity)
        {
            const bool Exists { Contains(Entity) };
            if (Exists) Erase(Entity);
            return Exists;
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr std::size_t Remove(IteratorType First, SentinelType Last)
        {
            std::size_t Count {};

            if constexpr (std::same_as<IteratorType, Iterator>)
            {
                while (First != Last)
                {
                    while (First != Last && !Contains(*First)) ++First;
                    const auto IteratorToErase { First };
                    while (First != Last && Contains(*First)) ++First;

                    Count += std::distance(IteratorToErase, First);
                    Erase(IteratorToErase, First);
                }
            }
            else
            {
                for (; First != Last; ++First)
                {
                    Count += Remove(*First);
                }
            }

            return Count;
        }

        constexpr void SwapElements(const EntityType Left, const EntityType Right)
        {
            SwapElementsAt(GetIndex(Left), GetIndex(Right));
        }

        constexpr virtual void SwapElementsAt(const std::size_t Left, const std::size_t Right)
        {
            EGG_ASSERT(Left < GetSize() && Right < GetSize(), "Index out of bounds");
            auto& From { Packed[Left] };
            auto& To { Packed[Right] };

            GetReference(From) = TraitsType::Combine(static_cast<typename TraitsType::EntityType>(Right), TraitsType::ToIntegral(From));
            GetReference(To) = TraitsType::Combine(static_cast<typename TraitsType::EntityType>(Left), TraitsType::ToIntegral(To));

            using std::swap;
            swap(Packed[Left], Packed[Right]);
        }

        constexpr virtual void Reserve(const std::size_t Capacity)
        {
            Packed.reserve(Capacity);
        }

        constexpr virtual void ShrinkToFit()
        {
            Packed.shrink_to_fit();
        }

        [[nodiscard]] constexpr bool Contains(const EntityType Entity) const noexcept
        {
            const auto* Element { GetPointer(Entity) };
            return Element && (TraitsType::ToVersionPart(Entity) ^ TraitsType::ToIntegral(*Element)) < TraitsType::EntityMask;
        }

        [[nodiscard]] constexpr std::size_t GetIndex(const EntityType Entity) const noexcept
        {
            EGG_ASSERT(Contains(Entity), "Set does not contain entity");
            return TraitsType::ToEntity(GetReference(Entity));
        }

        [[nodiscard]] constexpr VersionType GetVersion(const EntityType Entity) const noexcept
        {
            const auto* Element { GetPointer(Entity) };
            return Element ? TraitsType::ToVersion(*Element) : TraitsType::ToVersion(TraitsType::Tombstone);
        }

        [[nodiscard]] constexpr Iterator Find(const EntityType Entity) noexcept
        {
            return Contains(Entity) ? ToIterator(Entity) : End();
        }

        [[nodiscard]] constexpr ConstIterator Find(const EntityType Entity) const noexcept
        {
            return Contains(Entity) ? ToIterator(Entity) : End();
        }

        template <typename CompareType, typename SortType = decltype(std::ranges::sort), typename ProjectionType = std::identity>
        constexpr void Sort(CompareType Compare, SortType Sort = SortType {}, ProjectionType Projection = ProjectionType {})
        {
            SortCount(GetSize(), std::move(Compare), std::move(Sort), std::move(Projection));
        }

        template <typename CompareType, typename SortType = decltype(std::ranges::sort), typename ProjectionType = std::identity>
        constexpr void SortCount(const std::size_t Count,
                                 CompareType Compare, SortType Sort = SortType {}, ProjectionType Projection = ProjectionType {})
        {
            EGG_ASSERT(Count <= GetSize(), "Count of elements to sort exceeds the number of elements");

            Sort(Packed.rend() - Count, Packed.rend(), std::move(Compare), std::move(Projection));

            for (std::size_t Position = 0u; Position < Count; ++Position)
            {
                std::size_t Current { Position };
                std::size_t Next { GetIndex(Packed[Current]) };

                while (Current != Next)
                {
                    const auto Index { GetIndex(Packed[Next]) };
                    UpdateToPacked(Current, Next, Index);
                    Current = std::exchange(Next, Index);
                }
            }
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr Iterator SortAs(IteratorType First, SentinelType Last)
        {
            Iterator It { Begin() };

            for (const Iterator CurrentLast { End() };
                 It != CurrentLast && First != Last;
                 ++First)
            {
                if (const EntityType Other { *First }; Contains(Other))
                {
                    if (const EntityType Current { *It }; Current != Other)
                    {
                        SwapElements(Current, Other);
                    }

                    ++It;
                }
            }

            return It;
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr Iterator SortCountAs(const std::size_t Count, IteratorType First, SentinelType Last)
        {
            EGG_ASSERT(Count <= GetSize(), "Count of elements to sort exceeds the number of elements");

            Iterator It { End() - Count };

            for (const Iterator CurrentLast { End() };
                 It != CurrentLast && First != Last;
                 ++First)
            {
                if (const EntityType Other { *First }; Contains(Other) && GetIndex(Other) < Count)
                {
                    if (const EntityType Current { *It }; Current != Other)
                    {
                        SwapElements(Current, Other);
                    }

                    ++It;
                }
            }

            return It;
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            return Packed.size();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return Packed.empty();
        }

        [[nodiscard]] constexpr std::size_t GetExtent() const noexcept
        {
            return Sparse.GetExtent();
        }

        [[nodiscard]] constexpr ConstPointer GetEntityData() const noexcept
        {
            return Packed.data();
        }

        [[nodiscard]] constexpr EntityType operator[](const std::size_t Position) const noexcept
        {
            EGG_ASSERT(Position < GetSize(), "Index out of bounds");
            return Packed[Position];
        }

        [[nodiscard]] constexpr virtual std::size_t GetCapacity() const noexcept
        {
            return Packed.capacity();
        }

        [[nodiscard]] constexpr Iterator Begin() noexcept
        {
            return { Packed, GetSize() };
        }

        [[nodiscard]] constexpr ConstIterator Begin() const noexcept
        {
            return { Packed, GetSize() };
        }

        [[nodiscard]] constexpr ConstIterator ConstBegin() const noexcept
        {
            return Begin();
        }

        [[nodiscard]] constexpr Iterator End() noexcept
        {
            return { Packed, {} };
        }

        [[nodiscard]] constexpr ConstIterator End() const noexcept
        {
            return { Packed, {} };
        }

        [[nodiscard]] constexpr ConstIterator ConstEnd() const noexcept
        {
            return End();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseBegin() noexcept
        {
            return std::make_reverse_iterator(End());
        }

        [[nodiscard]] constexpr ConstReverseIterator ReverseBegin() const noexcept
        {
            return std::make_reverse_iterator(End());
        }

        [[nodiscard]] constexpr ConstReverseIterator ConstReverseBegin() const noexcept
        {
            return ReverseBegin();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseEnd() noexcept
        {
            return std::make_reverse_iterator(Begin());
        }

        [[nodiscard]] constexpr ConstReverseIterator ReverseEnd() const noexcept
        {
            return std::make_reverse_iterator(Begin());
        }

        [[nodiscard]] constexpr ConstReverseIterator ConstReverseEnd() const noexcept
        {
            return ReverseEnd();
        }

        [[nodiscard]] constexpr AllocatorType GetAllocator() const noexcept
        {
            return Packed.get_allocator();
        }

    protected:
        constexpr virtual void UpdateToPacked(
            const std::size_t CurrentIndex,
            [[maybe_unused]] const std::size_t LeftIndex,
            [[maybe_unused]] const std::size_t RightIndex)
        {
            GetReference(Packed[CurrentIndex]) = TraitsType::Combine(
                static_cast<typename TraitsType::EntityType>(CurrentIndex),
                TraitsType::ToIntegral(Packed[CurrentIndex])
            );
        }

        constexpr virtual Iterator TryEmplace(const EntityType Entity)
        {
            EGG_ASSERT(Entity != TraitsType::Tombstone, "The entity cannot be a tombstone");
            auto& Element { Assure(Entity) };
            EGG_ASSERT(Element == TraitsType::Tombstone, "Slot not available");
            auto Position { GetSize() };
            Packed.push_back(Entity);
            Element = TraitsType::Combine(static_cast<typename TraitsType::EntityType>(Position), TraitsType::ToIntegral(Entity));
            return End() - (Position + 1u);
        }

        constexpr virtual void Pop(Iterator First, Iterator Last)
        {
            for (; First != Last; ++First)
            {
                Erase(First);
            }
        }

        constexpr void Erase(Iterator It)
        {
            EGG_ASSERT(Contains(*It), "Sparse set does not contain entity to remove");
            auto& Index { GetReference(*It) };
            const auto EntityIndex { TraitsType::ToEntity(Index) };

            GetReference(Packed.back()) = TraitsType::Combine(EntityIndex, TraitsType::ToIntegral(Packed.back()));
            Packed[static_cast<std::size_t>(EntityIndex)] = Packed.back();

            Index = TraitsType::Tombstone;
            Packed.pop_back();
        }

    private:
        [[nodiscard]] constexpr ConstPointer GetPointer(const EntityType Entity) const
        {
            return Sparse.GetPointer(TraitsType::ToEntity(Entity));
        }

        [[nodiscard]] constexpr Reference GetReference(const EntityType Entity) const
        {
            return Sparse.GetReference(TraitsType::ToEntity(Entity));
        }

        [[nodiscard]] constexpr Reference Assure(const EntityType Entity)
        {
            return Sparse.Assure(TraitsType::ToEntity(Entity), TraitsType::Tombstone);
        }

        [[nodiscard]] constexpr Iterator ToIterator(const EntityType Entity)
        {
            return End() - (GetIndex(Entity) + 1u);
        }

        [[nodiscard]] constexpr ConstIterator ToIterator(const EntityType Entity) const
        {
            return End() - (GetIndex(Entity) + 1u);
        }

        SparseContainer Sparse;
        PackedContainer Packed;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_SPARSE_SET_FILE_SPARSE_SET_H
