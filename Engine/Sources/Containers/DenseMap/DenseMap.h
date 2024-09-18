#ifndef ENGINE_SOURCES_CONTAINERS_DENSE_MAP_FILE_DENSE_MAP_H
#define ENGINE_SOURCES_CONTAINERS_DENSE_MAP_FILE_DENSE_MAP_H

#include "./DenseMapIterator.h"
#include "./DenseMapLocalIterator.h"
#include "../../Containers/Container.h"
#include "../../Containers/DenseMap/DenseMapNode.h"
#include "../CompressedPair/CompressedPair.h"
#include "Math/Math.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

namespace egg::Containers
{
    template <typename KeyParameter, typename ValueParameter,
              typename HashParameter, typename KeyEqualParameter,
              ValidAllocator<std::pair<const KeyParameter, ValueParameter>> AllocatorParameter>
    class DenseMap
    {
        static constexpr float DefaultThreshold { 0.875f };
        static constexpr std::size_t MinimumCapacity { 8u };

        using AllocatorTraits = AllocatorTraits<AllocatorParameter>;

        using NodeType = DenseMapNode<KeyParameter, ValueParameter>;
        using SparseContainer = std::vector<std::size_t, typename AllocatorTraits::template rebind_alloc<std::size_t>>;
        using PackedContainer = std::vector<NodeType, typename AllocatorTraits::template rebind_alloc<NodeType>>;

    public:
        using AllocatorType = AllocatorParameter;
        using KeyType = KeyParameter;
        using MappedType = ValueParameter;
        using ValueType = std::pair<const KeyType, ValueParameter>;

        using HashType = HashParameter;
        using KeyEqualType = KeyEqualParameter;

        using Iterator = DenseMapIterator<typename PackedContainer::iterator>;
        using ConstIterator = DenseMapIterator<typename PackedContainer::const_iterator>;
        using ReverseIterator = DenseMapIterator<typename PackedContainer::reverse_iterator>;
        using ConstReverseIterator = DenseMapIterator<typename PackedContainer::const_reverse_iterator>;

        using LocalIterator = DenseMapLocalIterator<typename PackedContainer::iterator>;
        using ConstLocalIterator = DenseMapLocalIterator<typename PackedContainer::const_iterator>;

        DenseMap() : DenseMap { MinimumCapacity }
        {
        }

        explicit DenseMap(const AllocatorType& Allocator) : DenseMap { MinimumCapacity, Allocator }
        {
        }

        DenseMap(const std::size_t BucketCount, const AllocatorType& Allocator) : DenseMap { BucketCount, DefaultThreshold, Allocator }
        {
        }

        DenseMap(const std::size_t BucketCount, const float Threshold, const AllocatorType& Allocator)
            : DenseMap { BucketCount, HashType {}, KeyEqualType {}, Allocator, Threshold }
        {
        }

        DenseMap(const std::size_t BucketCount, const HashType& Hash, const AllocatorType& Allocator)
            : DenseMap { BucketCount, Hash, KeyEqualType {}, Allocator, DefaultThreshold }
        {
        }

        DenseMap(const std::size_t BucketCount, const KeyEqualType& KeyEqual, const AllocatorType& Allocator)
            : DenseMap { BucketCount, HashType {}, KeyEqual, Allocator, DefaultThreshold }
        {
        }

        explicit DenseMap(const std::size_t BucketCount,
                          const HashType& Hash = HashType {}, const KeyEqualType KeyEqual = KeyEqualType {},
                          const AllocatorType& Allocator = AllocatorType {},
                          const float Threshold = DefaultThreshold)
            : Sparse { Allocator, Hash },
              Packed { Allocator, KeyEqual },
              Threshold { Threshold }
        {
            Rehash(BucketCount);
        }

        DenseMap(const DenseMap&) = default;

        DenseMap(const DenseMap& Other, const AllocatorType& Allocator)
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

        DenseMap(DenseMap&&) noexcept (
            std::is_nothrow_move_constructible_v<CompressedPair<SparseContainer, HashType>> &&
            std::is_nothrow_move_constructible_v<CompressedPair<PackedContainer, KeyEqualType>>) = default;

        DenseMap(DenseMap&& Other, const AllocatorType& Allocator)
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

        ~DenseMap() noexcept = default;

        DenseMap& operator=(const DenseMap&) = default;

        DenseMap& operator=(DenseMap&&) noexcept (
            std::is_nothrow_move_assignable_v<CompressedPair<SparseContainer, HashType>> &&
            std::is_nothrow_move_assignable_v<CompressedPair<PackedContainer, KeyEqualType>>) = default;

        std::pair<Iterator, bool> Insert(const ValueType& Value)
        {
            return InsertOrDoNothing(Value.first, Value.second);
        }

        std::pair<Iterator, bool> Insert(ValueType&& Value)
        {
            return InsertOrDoNothing(std::move(Value.first), std::move(Value.second));
        }

        template <typename Arg> requires std::is_constructible_v<ValueType, Arg&&>
        std::pair<Iterator, bool> Insert(Arg&& Value)
        {
            return InsertOrDoNothing(std::forward<Arg>(Value).first, std::forward<Arg>(Value).second);
        }

        template <typename IteratorType>
        void Insert(IteratorType First, IteratorType Last)
        {
            for (; First != Last; ++First)
            {
                Insert(*First);
            }
        }

        template <typename Arg>
        std::pair<Iterator, bool> InsertOrAssign(const KeyType& Key, Arg&& Value)
        {
            return InsertOrOverwrite(Key, std::forward<Arg>(Value));
        }

        template <typename Arg>
        std::pair<Iterator, bool> InsertOrAssign(KeyType&& Key, Arg&& Value)
        {
            return InsertOrOverwrite(std::move(Key), std::forward<Arg>(Value));
        }

        template <typename... Args>
        std::pair<Iterator, bool> Emplace(Args&&... Arguments)
        {
            if constexpr (!sizeof...(Args))
            {
                return InsertOrDoNothing(KeyType {});
            }
            else if constexpr (sizeof...(Args) == 1u)
            {
                return InsertOrDoNothing(std::forward<Args>(Arguments).first..., std::forward<Args>(Arguments).second...);
            }
            else if constexpr (sizeof...(Args) == 2u)
            {
                return InsertOrDoNothing(std::forward<Args>(Arguments)...);
            }
            else
            {
                auto& Node { Packed.GetFirst().emplace_back(GetSize(), std::forward<Args>(Arguments)...) };
                const auto Index { GetBucketIndex(Node.Value.first) };

                if (auto It = FindInBucket(Node.Value.first, Index); It != End())
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
        std::pair<Iterator, bool> TryEmplace(const KeyType& Key, Args&&... Arguments)
        {
            return InsertOrDoNothing(Key, std::forward<Args>(Arguments)...);
        }

        template <typename... Args>
        std::pair<Iterator, bool> TryEmplace(KeyType&& Key, Args&&... Arguments)
        {
            return InsertOrDoNothing(std::move(Key), std::forward<Args>(Arguments)...);
        }

        Iterator Erase(ConstIterator Position)
        {
            const auto Difference { Position - ConstBegin() };
            Erase(Position->first);
            return Begin() + Difference;
        }

        bool Erase(const KeyType& Key)
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

        void Clear() noexcept
        {
            Sparse.GetFirst().clear();
            Packed.GetFirst().clear();
            Rehash(0u);
        }

        void Reserve(const std::size_t Count)
        {
            Packed.GetFirst().reserve(Count);
            Rehash(std::ceil(static_cast<float>(Count) / GetMaxLoadFactor()));
        }

        void Rehash(const std::size_t BucketCount)
        {
            const auto Value {
                std::max(
                    std::max(BucketCount, MinimumCapacity),
                    static_cast<std::size_t>(GetSize() / GetMaxLoadFactor())
                )
            };

            if (const auto Size = std::bit_ceil(Value); Size != GetBucketCount())
            {
                Sparse.GetFirst().resize(Size);

                for (auto&& Element : Sparse.GetFirst())
                {
                    Element = (std::numeric_limits<std::size_t>::max)();
                }

                for (std::size_t Position = 0, Last = GetSize(); Position < Last; ++Position)
                {
                    const auto Index { GetBucketIndex(Packed.GetFirst()[Position].Value.first) };
                    Packed.GetFirst()[Position].Next = std::exchange(Sparse.GetFirst()[Index], Position);
                }
            }
        }

        void SetLoadFactor(const float Value)
        {
            EGG_ASSERT(Value > 0.f, "Invalid load factor");
            Threshold = Value;
            Rehash(0u);
        }

        [[nodiscard]] Iterator Find(const KeyType& Key)
        {
            return FindInBucket(Key, GetBucketIndex(Key));
        }

        [[nodiscard]] ConstIterator Find(const KeyType& Key) const
        {
            return FindInBucket(Key, GetBucketIndex(Key));
        }

        template <typename Other> requires (Transparent<HashType> && Transparent<KeyEqualType>)
        [[nodiscard]] Iterator Find(const Other& Key)
        {
            return FindInBucket(Key, GetBucketIndex(Key));
        }

        template <typename Other> requires (Transparent<HashType> && Transparent<KeyEqualType>)
        [[nodiscard]] ConstIterator Find(const Other& Key) const
        {
            return FindInBucket(Key, GetBucketIndex(Key));
        }

        [[nodiscard]] MappedType& At(const KeyType& Key)
        {
            auto It { Find(Key) };
            EGG_ASSERT(It != End(), "Invalid key");
            return It->second;
        }

        [[nodiscard]] const MappedType& At(const KeyType& Key) const
        {
            auto It { Find(Key) };
            EGG_ASSERT(It != End(), "Invalid key");
            return It->second;
        }

        [[nodiscard]] MappedType& operator[](const KeyType& Key)
        {
            return InsertOrDoNothing(Key).first->second;
        }

        [[nodiscard]] MappedType& operator[](KeyType&& Key)
        {
            return InsertOrDoNothing(std::move(Key)).first->second;
        }

        [[nodiscard]] bool IsContains(const KeyType& Key) const
        {
            return Find(Key) != End();
        }

        template <typename Other> requires (Transparent<HashType> && Transparent<KeyEqualType>)
        [[nodiscard]] bool IsContains(const Other& Key) const
        {
            return Find(Key) != End();
        }

        [[nodiscard]] std::size_t GetSize() const noexcept
        {
            return Packed.GetFirst().size();
        }

        [[nodiscard]] std::size_t GetMaxSize() const noexcept
        {
            return Packed.GetFirst().max_size();
        }

        [[nodiscard]] bool IsEmpty() const noexcept
        {
            return Packed.GetFirst().empty();
        }

        [[nodiscard]] std::size_t GetBucketCount() const noexcept
        {
            return Sparse.GetFirst().size();
        }

        [[nodiscard]] std::size_t GetMaxBucketCount() const noexcept
        {
            return Sparse.GetFirst().max_size();
        }

        [[nodiscard]] std::size_t GetBucketSize(const std::size_t Index) const
        {
            return static_cast<std::size_t>(std::distance(BucketBegin(Index), BucketEnd()));
        }

        [[nodiscard]] std::size_t GetBucketIndex(const KeyType& Key) const noexcept
        {
            return Math::FastModulo(static_cast<std::size_t>(Sparse.GetSecond()(Key)), GetBucketCount());
        }

        [[nodiscard]] float GetLoadFactor() const noexcept
        {
            return GetSize() / static_cast<float>(GetBucketCount());
        }

        [[nodiscard]] float GetMaxLoadFactor() const noexcept
        {
            return Threshold;
        }

        [[nodiscard]] Iterator Begin() noexcept
        {
            return Iterator { Packed.GetFirst().begin() };
        }

        [[nodiscard]] ConstIterator Begin() const noexcept
        {
            return ConstIterator { Packed.GetFirst().begin() };
        }

        [[nodiscard]] ConstIterator ConstBegin() const noexcept
        {
            return Begin();
        }

        [[nodiscard]] Iterator End() noexcept
        {
            return Iterator { Packed.GetFirst().end() };
        }

        [[nodiscard]] ConstIterator End() const noexcept
        {
            return ConstIterator { Packed.GetFirst().end() };
        }

        [[nodiscard]] ConstIterator ConstEnd() const noexcept
        {
            return End();
        }

        [[nodiscard]] ReverseIterator ReverseBegin() noexcept
        {
            return ReverseIterator { Packed.GetFirst().rbegin() };
        }

        [[nodiscard]] ConstReverseIterator ReverseBegin() const noexcept
        {
            return ConstReverseIterator { Packed.GetFirst().rbegin() };
        }

        [[nodiscard]] ConstReverseIterator ConstReverseBegin() const noexcept
        {
            return ReverseBegin();
        }

        [[nodiscard]] ReverseIterator ReverseEnd() noexcept
        {
            return ReverseIterator { Packed.GetFirst().rend() };
        }

        [[nodiscard]] ConstReverseIterator ReverseEnd() const noexcept
        {
            return ConstReverseIterator { Packed.GetFirst().rend() };
        }

        [[nodiscard]] ConstReverseIterator ConstReverseEnd() const noexcept
        {
            return ReverseEnd();
        }

        [[nodiscard]] LocalIterator BucketBegin(const std::size_t Index)
        {
            return { Packed.GetFirst().begin(), Sparse.GetFirst()[Index] };
        }

        [[nodiscard]] ConstLocalIterator BucketBegin(const std::size_t Index) const
        {
            return { Packed.GetFirst().begin(), Sparse.GetFirst()[Index] };
        }

        [[nodiscard]] ConstLocalIterator BucketConstBegin(const std::size_t Index) const
        {
            return BucketBegin(Index);
        }

        [[nodiscard]] LocalIterator BucketEnd() noexcept
        {
            return { Packed.GetFirst().begin(), (std::numeric_limits<std::size_t>::max)() };
        }

        [[nodiscard]] ConstLocalIterator BucketEnd() const noexcept
        {
            return { Packed.GetFirst().begin(), (std::numeric_limits<std::size_t>::max)() };
        }

        [[nodiscard]] ConstLocalIterator BucketConstEnd() const noexcept
        {
            return BucketEnd();
        }

        [[nodiscard]] HashType GetHash() const
        {
            return Sparse.GetSecond();
        }

        [[nodiscard]] KeyEqualType GetKeyEqual() const
        {
            return Packed.GetSecond();
        }

        [[nodiscard]] constexpr AllocatorType GetAllocator() const noexcept
        {
            return Sparse.GetFirst().get_allocator();
        }

    private:
        template <typename Other, typename... Args>
        [[nodiscard]] std::pair<Iterator, bool> InsertOrDoNothing(Other&& Key, Args&&... Arguments)
        {
            const auto Index { GetBucketIndex(Key) };

            if (auto It = FindInBucket(Key, Index); It != End())
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
        [[nodiscard]] std::pair<Iterator, bool> InsertOrOverwrite(Other&& Key, Arg&& Value)
        {
            const auto Index { GetBucketIndex(Key) };

            if (auto It = FindInBucket(Key, Index); It != End())
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
        [[nodiscard]] Iterator FindInBucket(const Other& Key, const std::size_t Bucket)
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
        [[nodiscard]] ConstIterator FindInBucket(const Other& Key, const std::size_t Bucket) const
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

        void MoveAndPop(const std::size_t Position)
        {
            if (const auto Last = GetSize() - 1u; Position != Last)
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

        void RehashIfRequired()
        {
            if (GetSize() > GetBucketCount() * GetMaxLoadFactor())
            {
                Rehash(GetBucketCount() << 1u);
            }
        }

        CompressedPair<SparseContainer, HashType> Sparse;
        CompressedPair<PackedContainer, KeyEqualType> Packed;
        float Threshold;
    };
}

#endif // ENGINE_SOURCES_CONTAINERS_DENSE_MAP_FILE_DENSE_MAP_H
