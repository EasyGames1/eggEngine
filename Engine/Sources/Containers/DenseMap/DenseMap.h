#ifndef ENGINE_SOURCES_CONTAINERS_DENSE_MAP_FILE_DENSE_MAP_H
#define ENGINE_SOURCES_CONTAINERS_DENSE_MAP_FILE_DENSE_MAP_H

#include "./Internal/DenseMapIterator.h"
#include "./Internal/DenseMapLocalIterator.h"
#include "./Internal/DenseMapNode.h"

#include <Containers/Container.h>
#include <Containers/CompressedPair/CompressedPair.h>
#include <Math/Math.h>
#include <Types/Capabilities/Capabilities.h>

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <utility>
#include <vector>

namespace egg::Containers
{
    template <typename KeyParameter, typename ValueParameter,
              typename HashParameter = std::identity, typename KeyEqualParameter = std::equal_to<>,
              Types::ValidAllocator<std::pair<const KeyParameter, ValueParameter>> AllocatorParameter
              = std::allocator<std::pair<const KeyParameter, ValueParameter>>>
    class DenseMap
    {
        static constexpr float DefaultThreshold { 0.875f };
        static constexpr std::size_t MinimumCapacity { 8u };

        using ContainerAllocatorTraits = AllocatorTraits<AllocatorParameter>;

        using NodeType = Internal::DenseMapNode<KeyParameter, ValueParameter>;

        using SparseContainer = std::vector<std::size_t, typename ContainerAllocatorTraits::template rebind_alloc<std::size_t>>;
        using PackedContainer = std::vector<NodeType, typename ContainerAllocatorTraits::template rebind_alloc<NodeType>>;

        using SparsePair = CompressedPair<SparseContainer, HashParameter>;
        using PackedPair = CompressedPair<PackedContainer, KeyEqualParameter>;

    public:
        using AllocatorType = AllocatorParameter;
        using KeyType = KeyParameter;
        using MappedType = ValueParameter;
        using ValueType = std::pair<const KeyType, ValueParameter>;

        using HashType = HashParameter;
        using KeyEqualType = KeyEqualParameter;

        using Iterator = Internal::DenseMapIterator<typename PackedContainer::iterator>;
        using ConstIterator = Internal::DenseMapIterator<typename PackedContainer::const_iterator>;
        using ReverseIterator = Internal::DenseMapIterator<typename PackedContainer::reverse_iterator>;
        using ConstReverseIterator = Internal::DenseMapIterator<typename PackedContainer::const_reverse_iterator>;

        using LocalIterator = Internal::DenseMapLocalIterator<typename PackedContainer::iterator>;
        using ConstLocalIterator = Internal::DenseMapLocalIterator<typename PackedContainer::const_iterator>;


        constexpr DenseMap() : DenseMap { MinimumCapacity }
        {
        }

        constexpr explicit DenseMap(const AllocatorType& Allocator) : DenseMap { MinimumCapacity, Allocator }
        {
        }

        constexpr DenseMap(const std::size_t BucketCount, const AllocatorType& Allocator)
            : DenseMap { BucketCount, HashType {}, KeyEqualType {}, Allocator }
        {
        }

        constexpr DenseMap(const std::size_t BucketCount, const float Threshold, const AllocatorType& Allocator)
            : DenseMap { BucketCount, HashType {}, KeyEqualType {}, Allocator, Threshold }
        {
        }

        constexpr DenseMap(const std::size_t BucketCount, const HashType& Hash, const AllocatorType& Allocator)
            : DenseMap { BucketCount, Hash, KeyEqualType {}, Allocator }
        {
        }

        constexpr DenseMap(const std::size_t BucketCount, const KeyEqualType& KeyEqual, const AllocatorType& Allocator)
            : DenseMap { BucketCount, HashType {}, KeyEqual, Allocator }
        {
        }

        constexpr explicit DenseMap(const std::size_t BucketCount,
                                    const HashType& Hash = HashType {}, const KeyEqualType& KeyEqual = KeyEqualType {},
                                    const AllocatorType& Allocator = AllocatorType {},
                                    const float Threshold = DefaultThreshold)
            : Sparse { Allocator, Hash },
              Packed { Allocator, KeyEqual },
              Threshold { Threshold }
        {
            Rehash(BucketCount);
        }

        constexpr DenseMap(const DenseMap&) = default;

        constexpr DenseMap(const DenseMap& Other, const AllocatorType& Allocator)
            : Sparse {
                  std::piecewise_construct,
                  std::forward_as_tuple(Other.Sparse.GetFirst(), Allocator),
                  std::forward_as_tuple(Other.Sparse.GetSecond())
              },
              Packed {
                  std::piecewise_construct,
                  std::forward_as_tuple(Other.Packed.GetFirst(), Allocator),
                  std::forward_as_tuple(Other.Packed.GetSecond())
              },
              Threshold { Other.Threshold }
        {
        }

        constexpr DenseMap(DenseMap&&)
            noexcept (std::is_nothrow_move_constructible_v<SparsePair> && std::is_nothrow_move_constructible_v<PackedPair>)
        = default;

        constexpr DenseMap(DenseMap&& Other, const AllocatorType& Allocator)
            : Sparse {
                  std::piecewise_construct,
                  std::forward_as_tuple(std::move(Other.Sparse.GetFirst()), Allocator),
                  std::forward_as_tuple(std::move(Other.Sparse.GetSecond()))
              },
              Packed {
                  std::piecewise_construct,
                  std::forward_as_tuple(std::move(Other.Packed.GetFirst()), Allocator),
                  std::forward_as_tuple(std::move(Other.Packed.GetSecond()))
              },
              Threshold { Other.Threshold }
        {
        }

        constexpr ~DenseMap() noexcept = default;

        constexpr DenseMap& operator=(const DenseMap&) = default;

        constexpr DenseMap& operator=(DenseMap&&)
            noexcept (std::is_nothrow_move_assignable_v<SparsePair> && std::is_nothrow_move_assignable_v<PackedPair>) = default;

        constexpr friend void swap(DenseMap& Left, DenseMap& Right)
            noexcept(std::is_nothrow_swappable_v<SparsePair> && std::is_nothrow_swappable_v<PackedPair>)
        {
            using std::swap;
            swap(Left.Sparse, Right.Sparse);
            swap(Left.Packed, Right.Packed);
            swap(Left.Threshold, Right.Threshold);
        }

        template <typename Arg> requires std::constructible_from<ValueType, Arg&&>
        constexpr std::pair<Iterator, bool> Insert(Arg&& Value)
        {
            return InsertOrDoNothing(std::forward_like<Arg>(Value.first), std::forward_like<Arg>(Value.second));
        }

        template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType>
        constexpr void Insert(IteratorType First, SentinelType Last)
        {
            for (; First != Last; ++First)
            {
                Insert(*First);
            }
        }

        template <typename Arg>
        constexpr std::pair<Iterator, bool> InsertOrAssign(const KeyType& Key, Arg&& Value)
        {
            return InsertOrOverwrite(Key, std::forward<Arg>(Value));
        }

        template <typename Arg>
        constexpr std::pair<Iterator, bool> InsertOrAssign(KeyType&& Key, Arg&& Value)
        {
            return InsertOrOverwrite(std::move(Key), std::forward<Arg>(Value));
        }

        template <typename... Args>
        constexpr std::pair<Iterator, bool> Emplace([[maybe_unused]] Args&&... Arguments)
        {
            if constexpr (!sizeof...(Args))
            {
                return InsertOrDoNothing(KeyType {});
            }
            else if constexpr (sizeof...(Args) == 1u)
            {
                return InsertOrDoNothing(std::forward_like<Args>(Arguments.first)..., std::forward_like<Args>(Arguments.second)...);
            }
            else if constexpr (sizeof...(Args) == 2u)
            {
                return InsertOrDoNothing(std::forward<Args>(Arguments)...);
            }
            else
            {
                auto& Node { Packed.GetFirst().emplace_back(GetSize(), std::forward<Args>(Arguments)...) };
                const auto Index { GetBucketIndex(Node.Value.first) };

                if (const auto It { FindInBucket(Node.Value.first, Index) }; It != End())
                {
                    Packed.GetFirst().pop_back();
                    return std::make_pair(It, false);
                }

                std::swap(Node.Next, Sparse.GetFirst()[Index]);

                RehashIfRequired();

                return std::make_pair(--End(), true);
            }
        }

        template <typename... Args>
        constexpr std::pair<Iterator, bool> TryEmplace(const KeyType& Key, Args&&... Arguments)
        {
            return InsertOrDoNothing(Key, std::forward<Args>(Arguments)...);
        }

        template <typename... Args>
        constexpr std::pair<Iterator, bool> TryEmplace(KeyType&& Key, Args&&... Arguments)
        {
            return InsertOrDoNothing(std::move(Key), std::forward<Args>(Arguments)...);
        }

        constexpr Iterator Erase(ConstIterator Position)
        {
            const auto Difference { Position - ConstBegin() };
            Erase(Position->first);
            return Begin() + Difference;
        }

        constexpr bool Erase(const KeyType& Key)
        {
            for (std::size_t* Current = &Sparse.GetFirst()[GetBucketIndex(Key)];
                 *Current != (std::numeric_limits<std::size_t>::max)();
                 Current = &Packed.GetFirst()[*Current].Next)
            {
                if (Packed.GetSecond()(Packed.GetFirst()[*Current].Value.first, Key))
                {
                    const auto Index { *Current };
                    *Current = Packed.GetFirst()[*Current].Next;
                    MoveAndPop(Index);
                    return true;
                }
            }

            return false;
        }

        constexpr void Clear() noexcept
        {
            Sparse.GetFirst().clear();
            Packed.GetFirst().clear();
            Rehash(0u);
        }

        constexpr void Reserve(const std::size_t Count)
        {
            Packed.GetFirst().reserve(Count);
            Rehash(std::ceil(static_cast<float>(Count) / GetMaxLoadFactor()));
        }

        constexpr void Rehash(const std::size_t BucketCount)
        {
            const auto NewBucketCount {
                std::bit_ceil(std::max(
                    std::max(BucketCount, MinimumCapacity),
                    static_cast<std::size_t>(GetSize() / GetMaxLoadFactor())
                ))
            };

            if (NewBucketCount == GetBucketCount()) return;

            Sparse.GetFirst().resize(NewBucketCount);

            for (auto&& Element : Sparse.GetFirst())
            {
                Element = (std::numeric_limits<std::size_t>::max)();
            }

            for (std::size_t Position = 0u, Last = GetSize(); Position < Last; ++Position)
            {
                const auto Index { GetBucketIndex(Packed.GetFirst()[Position].Value.first) };
                Packed.GetFirst()[Position].Next = std::exchange(Sparse.GetFirst()[Index], Position);
            }
        }

        constexpr void SetLoadFactor(const float Value)
        {
            EGG_ASSERT(Value > 0.f, "Invalid load factor");
            Threshold = Value;
            Rehash(0u);
        }

        [[nodiscard]] constexpr Iterator Find(const KeyType& Key)
        {
            return FindInBucket(Key, GetBucketIndex(Key));
        }

        [[nodiscard]] constexpr ConstIterator Find(const KeyType& Key) const
        {
            return FindInBucket(Key, GetBucketIndex(Key));
        }

        template <typename Other>
        [[nodiscard]] constexpr Iterator Find(const Other& Key)
            requires (Types::Transparent<HashType> && Types::Transparent<KeyEqualType>)
        {
            return FindInBucket(Key, GetBucketIndex(Key));
        }

        template <typename Other>
        [[nodiscard]] constexpr ConstIterator Find(const Other& Key) const
            requires (Types::Transparent<HashType> && Types::Transparent<KeyEqualType>)
        {
            return FindInBucket(Key, GetBucketIndex(Key));
        }

        [[nodiscard]] constexpr MappedType& At(const KeyType& Key)
        {
            const auto It { Find(Key) };
            EGG_ASSERT(It != End(), "Invalid key");
            return It->second;
        }

        [[nodiscard]] constexpr const MappedType& At(const KeyType& Key) const
        {
            const auto It { Find(Key) };
            EGG_ASSERT(It != End(), "Invalid key");
            return It->second;
        }

        [[nodiscard]] constexpr MappedType& operator[](const KeyType& Key)
        {
            return InsertOrDoNothing(Key).first->second;
        }

        [[nodiscard]] constexpr MappedType& operator[](KeyType&& Key)
        {
            return InsertOrDoNothing(std::move(Key)).first->second;
        }

        [[nodiscard]] constexpr bool Contains(const KeyType& Key) const
        {
            return Find(Key) != End();
        }

        template <typename Other>
        [[nodiscard]] constexpr bool Contains(const Other& Key) const
            requires (Types::Transparent<HashType> && Types::Transparent<KeyEqualType>)
        {
            return Find(Key) != End();
        }

        [[nodiscard]] constexpr std::size_t GetSize() const noexcept
        {
            return Packed.GetFirst().size();
        }

        [[nodiscard]] constexpr std::size_t GetMaxSize() const noexcept
        {
            return Packed.GetFirst().max_size();
        }

        [[nodiscard]] constexpr bool Empty() const noexcept
        {
            return Packed.GetFirst().empty();
        }

        [[nodiscard]] constexpr std::size_t GetBucketCount() const noexcept
        {
            return Sparse.GetFirst().size();
        }

        [[nodiscard]] constexpr std::size_t GetMaxBucketCount() const noexcept
        {
            return Sparse.GetFirst().max_size();
        }

        [[nodiscard]] constexpr std::size_t GetBucketSize(const std::size_t Index) const
        {
            return static_cast<std::size_t>(std::distance(BucketBegin(Index), BucketEnd()));
        }

        [[nodiscard]] constexpr std::size_t GetBucketIndex(const KeyType& Key) const noexcept
        {
            return Math::FastModulo(static_cast<std::size_t>(Sparse.GetSecond()(Key)), GetBucketCount());
        }

        [[nodiscard]] constexpr float GetLoadFactor() const noexcept
        {
            return GetSize() / static_cast<float>(GetBucketCount());
        }

        [[nodiscard]] constexpr float GetMaxLoadFactor() const noexcept
        {
            return Threshold;
        }

        [[nodiscard]] constexpr Iterator Begin() noexcept
        {
            return Iterator { Packed.GetFirst().begin() };
        }

        [[nodiscard]] constexpr ConstIterator Begin() const noexcept
        {
            return ConstIterator { Packed.GetFirst().begin() };
        }

        [[nodiscard]] constexpr ConstIterator ConstBegin() const noexcept
        {
            return Begin();
        }

        [[nodiscard]] constexpr Iterator End() noexcept
        {
            return Iterator { Packed.GetFirst().end() };
        }

        [[nodiscard]] constexpr ConstIterator End() const noexcept
        {
            return ConstIterator { Packed.GetFirst().end() };
        }

        [[nodiscard]] constexpr ConstIterator ConstEnd() const noexcept
        {
            return End();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseBegin() noexcept
        {
            return ReverseIterator { Packed.GetFirst().rbegin() };
        }

        [[nodiscard]] constexpr ConstReverseIterator ReverseBegin() const noexcept
        {
            return ConstReverseIterator { Packed.GetFirst().rbegin() };
        }

        [[nodiscard]] constexpr ConstReverseIterator ConstReverseBegin() const noexcept
        {
            return ReverseBegin();
        }

        [[nodiscard]] constexpr ReverseIterator ReverseEnd() noexcept
        {
            return ReverseIterator { Packed.GetFirst().rend() };
        }

        [[nodiscard]] constexpr ConstReverseIterator ReverseEnd() const noexcept
        {
            return ConstReverseIterator { Packed.GetFirst().rend() };
        }

        [[nodiscard]] constexpr ConstReverseIterator ConstReverseEnd() const noexcept
        {
            return ReverseEnd();
        }

        [[nodiscard]] constexpr LocalIterator BucketBegin(const std::size_t Index)
        {
            return { Packed.GetFirst().begin(), Sparse.GetFirst()[Index] };
        }

        [[nodiscard]] constexpr ConstLocalIterator BucketBegin(const std::size_t Index) const
        {
            return { Packed.GetFirst().begin(), Sparse.GetFirst()[Index] };
        }

        [[nodiscard]] constexpr ConstLocalIterator BucketConstBegin(const std::size_t Index) const
        {
            return BucketBegin(Index);
        }

        [[nodiscard]] constexpr LocalIterator BucketEnd() noexcept
        {
            return { Packed.GetFirst().begin(), (std::numeric_limits<std::size_t>::max)() };
        }

        [[nodiscard]] constexpr ConstLocalIterator BucketEnd() const noexcept
        {
            return { Packed.GetFirst().begin(), (std::numeric_limits<std::size_t>::max)() };
        }

        [[nodiscard]] constexpr ConstLocalIterator BucketConstEnd() const noexcept
        {
            return BucketEnd();
        }

        [[nodiscard]] constexpr HashType GetHash() const
        {
            return Sparse.GetSecond();
        }

        [[nodiscard]] constexpr KeyEqualType GetKeyEqual() const
        {
            return Packed.GetSecond();
        }

        [[nodiscard]] constexpr AllocatorType GetAllocator() const noexcept
        {
            return Sparse.GetFirst().get_allocator();
        }

    private:
        template <typename Other, typename... Args>
        [[nodiscard]] constexpr std::pair<Iterator, bool> InsertOrDoNothing(Other&& Key, Args&&... Arguments)
        {
            const auto Index { GetBucketIndex(Key) };

            if (const auto It { FindInBucket(Key, Index) }; It != End())
            {
                return std::make_pair(It, false);
            }

            Packed.GetFirst().emplace_back(
                std::exchange(Sparse.GetFirst()[Index], GetSize()),
                std::piecewise_construct,
                std::forward_as_tuple(std::forward<Other>(Key)),
                std::forward_as_tuple(std::forward<Args>(Arguments)...)
            );

            RehashIfRequired();

            return std::make_pair(--End(), true);
        }

        template <typename Other, typename Arg>
        [[nodiscard]] constexpr std::pair<Iterator, bool> InsertOrOverwrite(Other&& Key, Arg&& Value)
        {
            const auto Index { GetBucketIndex(Key) };

            if (const auto It { FindInBucket(Key, Index) }; It != End())
            {
                It->second = std::forward<Arg>(Value);
                return std::make_pair(It, false);
            }

            Packed.GetFirst().emplace_back(
                std::exchange(Sparse.GetFirst()[Index], GetSize()),
                std::forward<Other>(Key), std::forward<Arg>(Value)
            );

            RehashIfRequired();

            return std::make_pair(--End(), true);
        }

        template <typename Other>
        [[nodiscard]] constexpr Iterator FindInBucket(const Other& Key, const std::size_t Bucket)
        {
            for (auto First = BucketBegin(Bucket), Last = BucketEnd(); First != Last; ++First)
            {
                if (Packed.GetSecond()(First->first, Key))
                {
                    return Begin() + static_cast<typename Iterator::difference_type>(First.GetIndex());
                }
            }

            return End();
        }

        template <typename Other>
        [[nodiscard]] constexpr ConstIterator FindInBucket(const Other& Key, const std::size_t Bucket) const
        {
            for (auto First = BucketBegin(Bucket), Last = BucketEnd(); First != Last; ++First)
            {
                if (Packed.GetSecond()(First->first, Key))
                {
                    return Begin() + static_cast<typename Iterator::difference_type>(First.GetIndex());
                }
            }

            return End();
        }

        constexpr void MoveAndPop(const std::size_t Position)
        {
            if (const auto Last { GetSize() - 1u }; Position != Last)
            {
                std::size_t* Current { &Sparse.GetFirst()[GetBucketIndex(Packed.GetFirst().back().Value.first)] };
                Packed.GetFirst()[Position] = std::move(Packed.GetFirst().back());

                for (; *Current != Last; Current = &Packed.GetFirst()[*Current].Next)
                {
                }

                *Current = Position;
            }

            Packed.GetFirst().pop_back();
        }

        constexpr void RehashIfRequired()
        {
            if (GetSize() > GetBucketCount() * GetMaxLoadFactor())
            {
                Rehash(GetBucketCount() << 1u);
            }
        }

        SparsePair Sparse;
        PackedPair Packed;
        float Threshold;
    };
}

#endif // ENGINE_SOURCES_CONTAINERS_DENSE_MAP_FILE_DENSE_MAP_H
