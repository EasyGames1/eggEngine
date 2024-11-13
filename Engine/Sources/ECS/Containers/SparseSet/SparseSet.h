#ifndef ENGINE_SOURCES_ECS_CONTAINERS_SPARSE_SET_FILE_SPARSE_SET_H
#define ENGINE_SOURCES_ECS_CONTAINERS_SPARSE_SET_FILE_SPARSE_SET_H

#include "../Container.h"
#include "../../Traits/EntityTraits.h"
#include "Algorithms/Sorting/StandardSorting.h"
#include "ECS/Containers/PagedVector/PagedVector.h"
#include "./Internal/SparseSetIterator.h"
#include "Type/Traits/Capabilities.h"

#include <memory>
#include <vector>

namespace egg::ECS::Containers
{
    template <ValidEntity Type, Types::ValidAllocator<Type> AllocatorParameter = std::allocator<Type>>
    class SparseSet
    {
        using ContainerAllocatorTraits = AllocatorTraits<AllocatorParameter>;

        using SparseContainer = PagedVector<Type, AllocatorParameter>;
        using PackedContainer = std::vector<Type, AllocatorParameter>;

        using TraitsType = EntityTraits<Type>;

        using Reference = typename PackedContainer::reference;

    public:
        using AllocatorType = AllocatorParameter;
        using ConstPointer = typename PackedContainer::const_pointer;

        using EntityType = typename TraitsType::ValueType;
        using VersionType = typename TraitsType::VersionType;

        using Iterator = Internal::SparseSetIterator<PackedContainer>;
        using ConstIterator = Internal::SparseSetIterator<const PackedContainer>;
        using ReverseIterator = std::reverse_iterator<Iterator>;
        using ConstReverseIterator = std::reverse_iterator<ConstIterator>;


        constexpr explicit SparseSet(const AllocatorType& Allocator = {})
            noexcept(std::is_nothrow_constructible_v<SparseContainer, const AllocatorType&>)
            : Sparse { Allocator }, Packed { Allocator }
        {
        }

        SparseSet(const SparseSet&) = delete;

        constexpr SparseSet(SparseSet&& Other) noexcept(std::is_nothrow_move_constructible_v<SparseContainer>) = default;

        constexpr SparseSet(SparseSet&& Other, const AllocatorType& Allocator) : Sparse { std::move(Other.Sparse), Allocator },
                                                                                 Packed { std::move(Other.Packed), Allocator }
        {
            EGG_ASSERT(ContainerAllocatorTraits::is_always_equal::value || Packed.get_allocator() == Other.Packed.get_allocator(),
                       "Cannot move sparse set because it has an incompatible allocator");
        }

        constexpr virtual ~SparseSet() noexcept = default;

        SparseSet& operator=(const SparseSet&) = delete;

        constexpr SparseSet& operator=(SparseSet&& Other) noexcept(std::is_nothrow_move_assignable_v<SparseContainer>)
        {
            EGG_ASSERT(ContainerAllocatorTraits::is_always_equal::value || Packed.get_allocator() == Other.Packed.get_allocator(),
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

        template <typename IteratorType>
        constexpr Iterator Push(IteratorType First, IteratorType Last)
        {
            auto Current { End() };

            for (; First != Last; ++First)
            {
                Current = TryEmplace(*First);
            }

            return Current;
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

        template <typename It>
        constexpr void Erase(It First, It Last)
        {
            if constexpr (std::same_as<It, Iterator>)
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

        template <typename It>
        constexpr std::size_t Remove(It First, It Last)
        {
            std::size_t Count {};

            if constexpr (std::same_as<It, Iterator>)
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
            SwapAt(GetIndex(Left), GetIndex(Right));
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

        [[nodiscard]] constexpr ConstIterator Find(const EntityType Entity) const noexcept
        {
            return Contains(Entity) ? ToIterator(Entity) : End();
        }

        [[nodiscard]] constexpr Iterator Find(const EntityType Entity) noexcept
        {
            return Contains(Entity) ? ToIterator(Entity) : End();
        }

        template <typename CompareType, typename SortType = Algorithms::Sorting::StandardSorting, typename... Args>
        constexpr void Sort(CompareType Compare, SortType Sort = SortType {}, Args&&... Arguments)
        {
            SortCount(GetSize(), std::move(Compare), std::move(Sort), std::forward<Args>(Arguments)...);
        }

        template <typename CompareType, typename SortType = Algorithms::Sorting::StandardSorting, typename... Args>
        constexpr void SortCount(const std::size_t Count, CompareType Compare, SortType Sort = SortType {}, Args&&... Arguments)
        {
            EGG_ASSERT(Count <= GetSize(), "Count of elements to sort exceeds the number of elements");

            Sort(Packed.rend() - Count, Packed.rend(), std::move(Compare), std::forward<Args>(Arguments)...);

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

        template <typename IteratorType>
        constexpr Iterator SortAs(IteratorType First, IteratorType Last)
        {
            auto It { Begin() };

            for (const auto CurrentLast { End() };
                 It != CurrentLast && First != Last;
                 ++First)
            {
                if (const auto Other { *First }; Contains(Other))
                {
                    if (const auto Current { *It }; Current != Other)
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
            EGG_ASSERT(Position < Packed.size(), "Index out of bounds");
            return Packed[Position];
        }

        [[nodiscard]] constexpr virtual std::size_t GetCapacity() const noexcept
        {
            return Packed.capacity();
        }

        [[nodiscard]] constexpr Iterator Begin() noexcept
        {
            return { Packed, Packed.size() };
        }

        [[nodiscard]] constexpr ConstIterator Begin() const noexcept
        {
            return { Packed, Packed.size() };
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
            EGG_ASSERT(Contains(*It), "Sparse does not contain entity to remove");
            auto& Index { GetReference(*It) };
            const auto EntityIndex { TraitsType::ToEntity(Index) };

            GetReference(Packed.back()) = TraitsType::Combine(EntityIndex, TraitsType::ToIntegral(Packed.back()));
            Packed[static_cast<std::size_t>(EntityIndex)] = Packed.back();

            Index = TraitsType::Tombstone;
            Packed.pop_back();
        }

        constexpr virtual void SwapAt(const std::size_t Left, const std::size_t Right)
        {
            auto& From { Packed[Left] };
            auto& To { Packed[Right] };

            GetReference(From) = TraitsType::Combine(static_cast<typename TraitsType::EntityType>(Right), TraitsType::ToIntegral(From));
            GetReference(To) = TraitsType::Combine(static_cast<typename TraitsType::EntityType>(Left), TraitsType::ToIntegral(To));

            using std::swap;
            swap(Packed[Left], Packed[Right]);
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
