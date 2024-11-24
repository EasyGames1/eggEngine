#include "../Single.h"

#include <Containers/DenseMap/DenseMap.h>
#include <gtest/gtest.h>

#include <ranges>

class DenseMapTest : public testing::Test
{
protected:
    using KeyType = std::size_t;
    using ValueType = EntityType;
    using Hash = std::identity;
    using KeyEqual = std::equal_to<>;
    using Allocator = std::allocator<std::pair<const KeyType, ValueType>>;
    using ConvertableToKey = unsigned long long;

    using DenseMapType = egg::Containers::DenseMap<KeyType, ValueType, Hash, KeyEqual, Allocator>;
    using IteratorType = DenseMapType::Iterator;

    DenseMapTest()
    {
        DenseMap.Reserve(IterationsCount);
        for (std::size_t i = 0u; i < IterationsCount; ++i)
        {
            DenseMap.Emplace(i, GetEntityAt(i));
        }
    }

    DenseMapType DenseMap;
};

TEST_F(DenseMapTest, Insert)
{
    for (std::size_t i = 0u; i < IterationsCount; ++i)
    {
        auto [It, IsInserted] { DenseMap.Insert({ i, GetEntityAt(i) }) };
        EXPECT_FALSE(IsInserted);
    }

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        auto [It, IsInserted] { DenseMap.Insert({ i, GetEntityAt(i) }) };
        ASSERT_TRUE(IsInserted);
        EXPECT_EQ(It, DenseMap.Find(i));
        EXPECT_EQ(It->first, i);
        EXPECT_EQ(It->second, GetEntityAt(i));
    }
    EXPECT_EQ(DenseMap.GetSize(), IterationsCount * 2u);
}

TEST_F(DenseMapTest, InsertIterators)
{
    std::vector<std::pair<const KeyType, ValueType>> Vector;
    Vector.reserve(IterationsCount);

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        Vector.emplace_back(i, GetEntityAt(i));
    }

    DenseMap.Insert(Vector.begin(), Vector.end());

    for (auto [Key, Value] : Vector)
    {
        EXPECT_TRUE(DenseMap.Contains(Key));
        EXPECT_EQ(DenseMap.At(Key), Value);
    }

    EXPECT_EQ(DenseMap.GetSize(), IterationsCount * 2u);
}

TEST_F(DenseMapTest, InsertOrAssign)
{
    for (std::size_t i = 0u; i < IterationsCount; ++i)
    {
        auto [It, IsInserted] { DenseMap.InsertOrAssign(i, GetEntityAt(i)) };
        EXPECT_FALSE(IsInserted);
    }

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        auto [It, IsInserted] { DenseMap.InsertOrAssign(i, GetEntityAt(i)) };
        ASSERT_TRUE(IsInserted);
        EXPECT_EQ(It, DenseMap.Find(i));
        EXPECT_EQ(It->first, i);
        EXPECT_EQ(It->second, GetEntityAt(i));
    }

    EXPECT_EQ(DenseMap.GetSize(), IterationsCount * 2u);
}

TEST_F(DenseMapTest, Emplace)
{
    for (std::size_t i = 0u; i < IterationsCount; ++i)
    {
        auto [It, IsInserted] { DenseMap.Emplace() };
        EXPECT_FALSE(IsInserted);
    }

    EXPECT_EQ(DenseMap.GetSize(), IterationsCount);

    for (std::size_t i = 0u; i < IterationsCount; ++i)
    {
        std::pair<const KeyType, ValueType> Pair { i, GetEntityAt(i) };
        auto [It, IsInserted] { DenseMap.Emplace(Pair) };
        EXPECT_FALSE(IsInserted);
    }

    EXPECT_EQ(DenseMap.GetSize(), IterationsCount);

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        std::pair<const KeyType, ValueType> Pair { i, GetEntityAt(i) };
        auto [It, IsInserted] { DenseMap.Emplace(Pair) };
        ASSERT_TRUE(IsInserted);
        EXPECT_EQ(It, DenseMap.Find(i));
        EXPECT_EQ(It->first, i);
        EXPECT_EQ(It->second, GetEntityAt(i));
    }

    EXPECT_EQ(DenseMap.GetSize(), IterationsCount * 2u);

    DenseMap.Clear();

    for (std::size_t i = 0u; i < IterationsCount; ++i)
    {
        auto [It, IsInserted] { DenseMap.Emplace(i, GetEntityAt(i)) };
        ASSERT_TRUE(IsInserted);
        EXPECT_EQ(It, DenseMap.Find(i));
        EXPECT_EQ(It->first, i);
        EXPECT_EQ(It->second, GetEntityAt(i));
    }

    EXPECT_EQ(DenseMap.GetSize(), IterationsCount);

    for (std::size_t i = 0u; i < IterationsCount; ++i)
    {
        auto [It, IsInserted] { DenseMap.Emplace(i, GetEntityAt(i)) };
        EXPECT_FALSE(IsInserted);
    }

    for (std::size_t i = 0u; i < IterationsCount; ++i)
    {
        auto [It, IsInserted] { DenseMap.Emplace(std::piecewise_construct, std::make_tuple(i), std::make_tuple(GetEntityAt(i))) };
        EXPECT_FALSE(IsInserted);
    }

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        auto [It, IsInserted] { DenseMap.Emplace(std::piecewise_construct, std::make_tuple(i), std::make_tuple(GetEntityAt(i))) };
        ASSERT_TRUE(IsInserted);
        EXPECT_EQ(It, DenseMap.Find(i));
        EXPECT_EQ(It->first, i);
        EXPECT_EQ(It->second, GetEntityAt(i));
    }

    EXPECT_EQ(DenseMap.GetSize(), IterationsCount * 2u);
}

TEST_F(DenseMapTest, TryEmplace)
{
    for (std::size_t i = 0u; i < IterationsCount; ++i)
    {
        auto [It, IsInserted] { DenseMap.TryEmplace(i, GetEntityAt(i)) };
        EXPECT_FALSE(IsInserted);
    }

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        auto [It, IsInserted] { DenseMap.TryEmplace(i, GetEntityAt(i)) };
        EXPECT_TRUE(IsInserted);
        EXPECT_EQ(It, DenseMap.Find(i));
        EXPECT_EQ(It->first, i);
        EXPECT_EQ(It->second, GetEntityAt(i));
    }
}

TEST_F(DenseMapTest, Erase)
{
    EXPECT_FALSE(DenseMap.Erase(IterationsCount));

    for (std::size_t i = 0u; i < IterationsCount; ++i)
    {
        EXPECT_TRUE(DenseMap.Erase(i));
        EXPECT_FALSE(DenseMap.Contains(i));
    }

    EXPECT_TRUE(DenseMap.Empty());
}

TEST_F(DenseMapTest, EraseIterator)
{
    for (auto It = DenseMap.Begin(); It != DenseMap.End();)
    {
        const auto KeyToErase { It->first };
        It = DenseMap.Erase(DenseMapType::ConstIterator { It });
        EXPECT_FALSE(DenseMap.Contains(KeyToErase));
    }
}

TEST_F(DenseMapTest, Clear)
{
    DenseMap.Clear();
    EXPECT_TRUE(DenseMap.Empty());
}

TEST_F(DenseMapTest, Rehash)
{
    const std::size_t RehashTo { std::max(DenseMap.GetBucketCount(), IterationsCount * 2u) };
    DenseMap.Rehash(RehashTo);
    EXPECT_EQ(DenseMap.GetBucketCount(), std::bit_ceil(RehashTo));
}

TEST_F(DenseMapTest, Find)
{
    EXPECT_EQ(DenseMap.Find(IterationsCount), DenseMap.End());

    for (auto It = DenseMap.Begin(), End = DenseMap.End(); It != End; ++It)
    {
        EXPECT_EQ(DenseMap.Find(It->first), It);
        EXPECT_EQ(DenseMap.Find(static_cast<ConvertableToKey>(It->first)), It);
    }
}

TEST_F(DenseMapTest, At)
{
    for (auto [Key, Value] : DenseMap)
    {
        EXPECT_EQ(DenseMap.At(Key), Value);
    }
}

TEST_F(DenseMapTest, Subscript)
{
    for (auto [Key, Value] : DenseMap)
    {
        EXPECT_EQ(DenseMap[Key], Value);
    }

    EXPECT_FALSE(DenseMap.Contains(IterationsCount));

    DenseMap[IterationsCount] = GetEntityAt(IterationsCount);

    EXPECT_TRUE(DenseMap.Contains(IterationsCount));
}

TEST_F(DenseMapTest, IsContains)
{
    for (const auto Key : DenseMap | std::views::keys)
    {
        EXPECT_TRUE(DenseMap.Contains(Key));
    }

    for (const auto Key : DenseMap | std::views::keys)
    {
        EXPECT_TRUE(DenseMap.Contains(static_cast<ConvertableToKey>(Key)));
    }
}

TEST_F(DenseMapTest, GetSize)
{
    EXPECT_EQ(DenseMap.GetSize(), IterationsCount);
}

TEST_F(DenseMapTest, IsEmpty)
{
    EXPECT_FALSE(DenseMap.Empty());

    DenseMap.Clear();

    EXPECT_TRUE(DenseMap.Empty());
}

TEST_F(DenseMapTest, Iterables)
{
    for (auto [Key, Value] : DenseMap)
    {
        EXPECT_EQ(DenseMap.At(Key), Value);
        EXPECT_EQ(Value, GetEntityAt(Key));
    }

    for (auto It = DenseMap.ReverseBegin(), End = DenseMap.ReverseEnd(); It != End; ++It)
    {
        auto [Key, Value] { *It };
        EXPECT_EQ(DenseMap.At(Key), Value);
        EXPECT_EQ(Value, GetEntityAt(Key));
    }
}
