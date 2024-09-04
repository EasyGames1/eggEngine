#include "ECS/Containers/SparseSet/SparseSet.h"

#include "./Single.h"

#include "TypeInfo/TypeInfo.h"

#include <gtest/gtest.h>


class SparseSetTest : public testing::Test
{
protected:
    using SparseSetType = egg::ECS::Containers::SparseSet<EntityType>;
    using TraitsType = egg::ECS::EntityTraits<EntityType>;

    SparseSetTest()
    {
        for (std::size_t i = 0; i < IterationsCount; ++i)
        {
            Sparse.Push(TraitsType::Construct(i, i));
        }
    }

    SparseSetType Sparse;
};

TEST_F(SparseSetTest, Push)
{
    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        EntityType NewEntity { GetEntityAt(i) };
        EXPECT_EQ(*Sparse.Push(NewEntity), NewEntity);
    }
    EXPECT_EQ(Sparse.GetSize(), IterationsCount * 2u);
}

TEST_F(SparseSetTest, PushIterators)
{
    std::vector<EntityType> Vector;

    Vector.reserve(IterationsCount);
    Sparse.Reserve(IterationsCount);

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        Vector.emplace_back(GetEntityAt(i));
    }

    Sparse.Push(Vector.begin(), Vector.end());

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        EXPECT_TRUE(Sparse.IsContains(GetEntityAt(i)));
        EXPECT_EQ(Vector[i - IterationsCount], Sparse[i]);
    }
}

TEST_F(SparseSetTest, UpdateVersion)
{
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        const EntityType CurrentEntity { GetEntityAt(i) };
        const TraitsType::VersionType Version { Sparse.GetVersion(CurrentEntity) };
        const TraitsType::VersionType NewVersion { static_cast<TraitsType::VersionType>(Version + 1u) };
        EXPECT_EQ(Sparse.UpdateVersion(TraitsType::Construct(TraitsType::ToEntity(CurrentEntity), NewVersion)), NewVersion);
        EXPECT_EQ(Sparse.GetVersion(CurrentEntity), NewVersion);
    }
}

TEST_F(SparseSetTest, Erase)
{
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        const EntityType ToErase { GetEntityAt(i) };
        EXPECT_TRUE(Sparse.IsContains(ToErase));
        Sparse.Erase(ToErase);
        EXPECT_FALSE(Sparse.IsContains(ToErase));
    }
}

TEST_F(SparseSetTest, EraseIterators)
{
    std::vector<EntityType> Vector;
    Vector.reserve(IterationsCount);

    for (auto Current : Sparse)
    {
        Vector.emplace_back(Current);
    }

    Sparse.Erase(Vector.begin(), Vector.end());

    for (const auto Current : Vector)
    {
        EXPECT_FALSE(Sparse.IsContains(Current));
    }

    EXPECT_TRUE(Sparse.IsEmpty());
}

TEST_F(SparseSetTest, Clear)
{
    Sparse.Clear();
    EXPECT_TRUE(Sparse.IsEmpty());
}

TEST_F(SparseSetTest, Remove)
{
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        const EntityType EntityToRemove { GetEntityAt(i) };
        EXPECT_TRUE(Sparse.Remove(EntityToRemove));
    }

    EXPECT_TRUE(Sparse.IsEmpty());
}

TEST_F(SparseSetTest, RemoveIterators)
{
    std::vector<EntityType> Vector;
    Vector.reserve(IterationsCount);

    for (auto Current : Sparse)
    {
        Vector.emplace_back(Current);
    }

    EXPECT_EQ(Sparse.Remove(Vector.begin(), Vector.end()), IterationsCount);

    for (const auto Current : Vector)
    {
        EXPECT_FALSE(Sparse.IsContains(Current));
    }

    EXPECT_TRUE(Sparse.IsEmpty());
}

TEST_F(SparseSetTest, SwapElements)
{
    for (std::size_t i = 1; i < IterationsCount; ++i)
    {
        const EntityType PreviousEntity { GetEntityAt(i - 1u) };
        const EntityType CurrentEntity { GetEntityAt(i) };

        const std::size_t PreviousIndex { Sparse.GetIndex(PreviousEntity) };
        const std::size_t CurrentIndex { Sparse.GetIndex(CurrentEntity) };

        Sparse.SwapElements(PreviousEntity, CurrentEntity);

        EXPECT_EQ(Sparse.GetIndex(CurrentEntity), PreviousIndex);
        EXPECT_EQ(Sparse.GetIndex(PreviousEntity), CurrentIndex);

        Sparse.SwapElements(PreviousEntity, CurrentEntity);

        EXPECT_EQ(Sparse.GetIndex(CurrentEntity), CurrentIndex);
        EXPECT_EQ(Sparse.GetIndex(PreviousEntity), PreviousIndex);

        Sparse.SwapElements(CurrentEntity, PreviousEntity);

        EXPECT_EQ(Sparse.GetIndex(CurrentEntity), PreviousIndex);
        EXPECT_EQ(Sparse.GetIndex(PreviousEntity), CurrentIndex);

        Sparse.SwapElements(CurrentEntity, PreviousEntity);

        EXPECT_EQ(Sparse.GetIndex(CurrentEntity), CurrentIndex);
        EXPECT_EQ(Sparse.GetIndex(PreviousEntity), PreviousIndex);
    }
}

TEST_F(SparseSetTest, GetSize)
{
    EXPECT_EQ(Sparse.GetSize(), IterationsCount);
}

TEST_F(SparseSetTest, GetExtent)
{
    EXPECT_EQ(Sparse.GetExtent(), ((IterationsCount - 1u) / TraitsType::PageSize + 1u) * TraitsType::PageSize);
}

TEST_F(SparseSetTest, GetData)
{
    auto* First { Sparse.GetData() };
    for (std::size_t i = 0; i < IterationsCount; ++i, ++First)
    {
        EXPECT_EQ(*First, GetEntityAt(i));
    }
}

TEST_F(SparseSetTest, IsContains)
{
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        EXPECT_TRUE(Sparse.IsContains(GetEntityAt(i)));
    }
}

TEST_F(SparseSetTest, GetIndex)
{
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        const EntityType EntityAtIndex { GetEntityAt(i) };
        EXPECT_EQ(Sparse[Sparse.GetIndex(EntityAtIndex)], EntityAtIndex);
    }
}

TEST_F(SparseSetTest, GetVersion)
{
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        const EntityType CurrentEntity { GetEntityAt(i) };
        EXPECT_EQ(Sparse.GetVersion(CurrentEntity), TraitsType::ToVersion(CurrentEntity));
    }
}

TEST_F(SparseSetTest, Find)
{
    EXPECT_EQ(Sparse.Find(GetEntityAt(IterationsCount)), Sparse.End());
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        EntityType CurrentEntity { GetEntityAt(i) };
        auto Found { Sparse.Find(CurrentEntity) };
        EXPECT_EQ(*Found, CurrentEntity);
        EXPECT_EQ(*Found, Sparse[Sparse.GetIndex(CurrentEntity)]);
    }
}

TEST_F(SparseSetTest, Sort)
{
    using Compare = std::less<>;

    for (std::size_t i = IterationsCount * 2u - 1u; i >= IterationsCount; --i)
    {
        Sparse.Push(GetEntityAt(i));
    }

    Sparse.Sort(Compare {});

    for (std::size_t i = IterationsCount - 1u; i; --i)
    {
        EXPECT_TRUE(Compare{}(Sparse[i], Sparse[i - 1]));
    }

    Sparse.Clear();

    for (std::size_t i = 0, Value = 1; i < IterationsCount; ++i)
    {
        Sparse.Push(GetEntityAt(Value));
        if (i % 2) --Value;
        else Value += 4u;
    }

    Sparse.Sort(Compare {});

    for (std::size_t i = IterationsCount - 1u; i; --i)
    {
        EXPECT_TRUE(Compare{}(Sparse[i], Sparse[i - 1]));
    }

    Sparse.Clear();

    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        Sparse.Push(GetEntityAt(i));
    }

    Sparse.Sort(Compare {});

    for (std::size_t i = IterationsCount - 1u; i; --i)
    {
        EXPECT_TRUE(Compare{}(Sparse[i], Sparse[i - 1]));
    }

    Sparse.Clear();

    std::vector<EntityType> Entities;
    Entities.reserve(IterationsCount);

    for (std::size_t i = 0, Value = 1; i < IterationsCount; ++i)
    {
        Entities.emplace_back(GetEntityAt(i));
        Sparse.Push(GetEntityAt(Value));
        if (i % 2) --Value;
        else Value += 4u;
    }

    Sparse.SortAs(Entities.begin(), Entities.end());

    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        if (auto First { Sparse.Find(Entities[i]) }; First != Sparse.End())
        {
            for (std::size_t j = i + 1; j < IterationsCount; ++j)
            {
                if (auto Second { Sparse.Find(Entities[j]) }; Second != Sparse.End())
                {
                    EXPECT_FALSE(Compare{}(First.GetIndex(), Second.GetIndex()));
                }
            }
        }
    }

    Entities.clear();

    for (std::size_t i = IterationsCount * 2u - 1u; i >= IterationsCount; --i)
    {
        Entities.emplace_back(GetEntityAt(i));
    }

    Sparse.SortAs(Entities.begin(), Entities.end());

    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        if (auto First { Sparse.Find(Entities[i]) }; First != Sparse.End())
        {
            for (std::size_t j = i + 1; j < IterationsCount; ++j)
            {
                if (auto Second { Sparse.Find(Entities[j]) }; Second != Sparse.End())
                {
                    EXPECT_FALSE(Compare{}(First.GetIndex(), Second.GetIndex()));
                }
            }
        }
    }

    union h
    {
    };

    enum hh
    {
    };

    std::cout << '"' << egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<h>() << '"' << '\n';
    std::cout << egg::TypeInfo::TypeInfo<std::uint64_t>::GetID<h>() << '\t';
    std::cout << std::hash<std::string_view> {}(egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<h>()) << '\n';

    std::cout << '"' << egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<hh>() << '"' << '\n';
    std::cout << egg::TypeInfo::TypeInfo<std::uint64_t>::GetID<hh>() << '\t';
    std::cout << std::hash<std::string_view> {}(egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<hh>()) << '\n';

    std::cout << '"' << egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<SparseSetType>() << '"' << '\n';
    std::cout << egg::TypeInfo::TypeInfo<std::uint64_t>::GetID<SparseSetType>() << '\t';
    std::cout << std::hash<std::string_view> {}(egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<SparseSetType>()) << '\n';

    std::cout << '"' << egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<EntityType>() << '"' << '\n';
    std::cout << egg::TypeInfo::TypeInfo<std::uint64_t>::GetID<EntityType>() << '\t';
    std::cout << std::hash<std::string_view> {}(egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<EntityType>()) << '\n';

    std::cout << '"' << egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<int>() << '"' << '\n';
    std::cout << egg::TypeInfo::TypeInfo<std::uint64_t>::GetID<int>() << '\t';
    std::cout << std::hash<std::string_view> {}(egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<int>()) << '\n';

    std::cout << '"' << egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<double>() << '"' << '\n';
    std::cout << egg::TypeInfo::TypeInfo<std::uint64_t>::GetID<double>() << '\t';
    std::cout << std::hash<std::string_view> {}(egg::TypeInfo::TypeInfo<std::uint64_t>::GetName<double>()) << '\n';
}

TEST_F(SparseSetTest, Subscript)
{
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        EXPECT_EQ(Sparse[i], GetEntityAt(i));
    }
}

TEST_F(SparseSetTest, Iterable)
{
    for (auto It = Sparse.Begin(), End = Sparse.End(); It != End; ++It)
    {
        EXPECT_EQ(*It, GetEntityAt(It.GetIndex()));
    }

    std::size_t i { 0 };
    for (auto It = Sparse.ReverseBegin(), End = Sparse.ReverseEnd(); It != End; ++It, ++i)
    {
        EXPECT_EQ(*It, GetEntityAt(i));
    }
}

class SparseSetIteratorTest : public SparseSetTest
{
protected:
    using IteratorType = SparseSetType::Iterator;

    SparseSetIteratorTest() :
        Begin { Sparse.Begin() },
        End { Sparse.End() },
        Between { std::next(Sparse.Begin(), std::distance(Begin, End) / 2u) }
    {
    }

    const IteratorType Begin;
    const IteratorType End;
    const IteratorType Between;
};

TEST_F(SparseSetIteratorTest, NotEqual)
{
    EXPECT_NE(Begin, End);
    EXPECT_NE(Begin, Between);
    EXPECT_NE(Between, End);
}

TEST_F(SparseSetIteratorTest, Dereference)
{
    auto It { Begin };
    for (std::size_t i = 0; i < IterationsCount; ++i, ++It)
    {
        EXPECT_EQ(*It, GetEntityAtReversed(i));
    }
}

TEST_F(SparseSetIteratorTest, PreIncrement)
{
    auto It { Begin };
    for (std::size_t i = 0; i < IterationsCount - 1u; ++i)
    {
        EXPECT_EQ(*++It, GetEntityAtReversed(i + 1));
    }
    EXPECT_EQ(It, std::prev(End));
}

TEST_F(SparseSetIteratorTest, PostIncrement)
{
    auto It { Begin };
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        EXPECT_EQ(*It++, GetEntityAtReversed(i));
    }
    EXPECT_EQ(It, End);
}

TEST_F(SparseSetIteratorTest, PreDecrement)
{
    auto It { End };
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        EXPECT_EQ(*--It, GetEntityAt(i));
    }
    EXPECT_EQ(It, Begin);
}

TEST_F(SparseSetIteratorTest, PostDecrement)
{
    auto It { std::prev(End) };
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        EXPECT_EQ(*It--, GetEntityAt(i));
    }
    EXPECT_EQ(std::next(It), Begin);
}

TEST_F(SparseSetIteratorTest, AdditionAssignment)
{
    auto It { Begin };
    It += std::distance(It, std::prev(End));
    EXPECT_EQ(It, std::prev(End));
}

TEST_F(SparseSetIteratorTest, Addition)
{
    EXPECT_EQ(Begin + (std::distance(Begin, End) - 1u), std::prev(End));
}

TEST_F(SparseSetIteratorTest, SubtractionAssignment)
{
    auto It { End };
    It -= std::distance(Begin, End);
    EXPECT_EQ(It, Begin);
}

TEST_F(SparseSetIteratorTest, Subtraction)
{
    EXPECT_EQ(End - (std::distance(Begin, End)), Begin);
}

TEST_F(SparseSetIteratorTest, Subscript)
{
    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        EXPECT_EQ(End[i + 1u], Sparse[i]);
    }
}

TEST_F(SparseSetIteratorTest, LessThan)
{
    EXPECT_LT(Begin, End);
    EXPECT_LT(Begin, Between);
    EXPECT_LT(Between, End);
}

TEST_F(SparseSetIteratorTest, GreaterThan)
{
    EXPECT_GT(End, Begin);
    EXPECT_GT(End, Between);
    EXPECT_GT(Between, Begin);
}

TEST_F(SparseSetIteratorTest, GreaterThanOrEqualTo)
{
    EXPECT_GE(End, Begin);
    EXPECT_GE(End, Between);
    EXPECT_GE(Between, Begin);
    EXPECT_GE(Begin, Begin);
    EXPECT_GE(End, End);
    EXPECT_GE(Between, Between);
}

TEST_F(SparseSetIteratorTest, LessThanOrEqualTo)
{
    EXPECT_LE(Begin, End);
    EXPECT_LE(Begin, Between);
    EXPECT_LE(Between, End);
    EXPECT_LE(Begin, Begin);
    EXPECT_LE(End, End);
    EXPECT_LE(Between, Between);
}
