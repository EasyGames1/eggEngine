#include "ECS/Containers/Storage/Storage.h"
#include "./Single.h"

#include <gtest/gtest.h>

struct Velocity
{
    bool operator==(const Velocity& Other) const
    {
        return X == Other.X && Y == Other.Y;
    }

    bool operator!=(const Velocity& Other) const
    {
        return !(*this == Other);
    }

    Velocity& operator+=(const float Value)
    {
        X += Value;
        Y += Value;
        return *this;
    }

    Velocity& operator-=(const float Value)
    {
        X -= Value;
        Y -= Value;
        return *this;
    }

    Velocity& operator*=(const float Value)
    {
        X *= Value;
        Y *= Value;
        return *this;
    }

    Velocity& operator/=(const float Value)
    {
        X /= Value;
        Y /= Value;
        return *this;
    }

    Velocity operator+(const float Value) const
    {
        return Velocity { *this } += Value;
    }

    Velocity operator-(const float Value) const
    {
        return *this + -Value;
    }

    Velocity operator*(const float Value) const
    {
        return Velocity { *this } *= Value;
    }

    Velocity operator/(const float Value) const
    {
        return *this * (1 / Value);
    }

    bool operator<(const Velocity& Other) const
    {
        return GetMagnitude() < Other.GetMagnitude();
    }

    [[nodiscard]] float GetMagnitude() const
    {
        return std::sqrt(X * X + Y * Y);
    }

    float X {};
    float Y {};
};

std::ostream& operator<<(std::ostream& Stream, const Velocity& Value)
{
    Stream << '(' << Value.X << ", " << Value.Y << ')';
    return Stream;
}

class StorageTest : public testing::Test
{
protected:
    using ComponentType = Velocity;
    using TraitsType = egg::ECS::ComponentTraits<ComponentType>;

    using StorageType = egg::ECS::Containers::Storage<ComponentType, EntityType>;

    static ComponentType GetComponentAt(const std::size_t Index)
    {
        return {
            static_cast<float>(Index + 1u),
            static_cast<float>(Index + 2u)
        };
    }

    static ComponentType GetComponentAtReversed(const std::size_t Index)
    {
        return GetComponentAt(GetReversedIndex(Index));
    }

    StorageTest()
    {
        for (std::size_t i = 0; i < IterationsCount; ++i)
        {
            Storage.Emplace(GetEntityAt(i), GetComponentAt(i));
        }
    }

    StorageType Storage;
};


TEST_F(StorageTest, Emplace)
{
    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        ComponentType NewComponent { GetComponentAt(i) };
        EXPECT_EQ(Storage.Emplace(GetEntityAt(i), NewComponent), NewComponent);
    }
    EXPECT_EQ(Storage.GetSize(), IterationsCount * 2u);
}

TEST_F(StorageTest, InsertDefault)
{
    std::vector<EntityType> Entities;

    Entities.reserve(IterationsCount);

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        Entities.emplace_back(GetEntityAt(i));
    }

    const ComponentType ComponentToInsert { GetComponentAt(IterationsCount) };

    Storage.Insert(Entities.begin(), Entities.end(), ComponentToInsert);

    for (const auto Current : Entities)
    {
        EXPECT_EQ(Storage.Get(Current), ComponentToInsert);
    }

    EXPECT_EQ(Storage.GetSize(), IterationsCount * 2u);
}

TEST_F(StorageTest, Insert)
{
    std::vector<EntityType> Entities;
    std::vector<ComponentType> Components;

    Entities.reserve(IterationsCount);
    Components.reserve(IterationsCount);

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        Entities.emplace_back(GetEntityAt(i));
        Components.emplace_back(GetComponentAt(i));
    }

    Storage.Insert(Entities.begin(), Entities.end(), Components.begin());

    for (std::size_t i = IterationsCount; i < IterationsCount * 2u; ++i)
    {
        EXPECT_EQ(Storage.Get(Entities[i - IterationsCount]), Components[i - IterationsCount]);
    }

    EXPECT_EQ(Storage.GetSize(), IterationsCount * 2u);
}

TEST_F(StorageTest, Patch)
{
    for (const auto Current : Storage)
    {
        ComponentType PatchedComponent { Storage.Get(Current) + IterationsCount };
        EXPECT_EQ(Storage.Patch(
                      Current,
                      [](ComponentType& Component){ Component += IterationsCount; }),
                  PatchedComponent);
    }

    for (const auto Current : Storage)
    {
        ComponentType PatchedComponent { (Storage.Get(Current) + IterationsCount) * 2u };
        EXPECT_EQ(Storage.Patch(
                      Current,
                      [](ComponentType& Component){ Component += IterationsCount; },
                      [](ComponentType& Component){ Component *= 2u; }),
                  PatchedComponent);
    }
}

TEST_F(StorageTest, Sort)
{
    using CompareFunction = std::less<ComponentType>;
    auto Compare {
        [this](const EntityType Left, const EntityType Right)
        {
            return CompareFunction {}(Storage.Get(Left), Storage.Get(Right));
        }
    };

    for (std::size_t i = IterationsCount * 2u - 1u; i >= IterationsCount; --i)
    {
        Storage.Emplace(GetEntityAt(i), GetComponentAt(i));
    }

    Storage.Sort(Compare);

    for (std::size_t i = IterationsCount - 1u; i; --i)
    {
        EXPECT_TRUE(Compare(Storage[i], Storage[i - 1]));
    }

    Storage.Clear();

    for (std::size_t i = 0, Value = 1; i < IterationsCount; ++i)
    {
        Storage.Emplace(GetEntityAt(Value), GetComponentAt(Value));
        if (i % 2) --Value;
        else Value += 4u;
    }

    Storage.Sort(Compare);

    for (std::size_t i = IterationsCount - 1u; i; --i)
    {
        EXPECT_TRUE(Compare(Storage[i], Storage[i - 1]));
    }

    Storage.Clear();

    for (std::size_t i = 0; i < IterationsCount; ++i)
    {
        Storage.Emplace(GetEntityAt(i), GetComponentAt(i));
    }

    Storage.Sort(Compare);

    for (std::size_t i = IterationsCount - 1u; i; --i)
    {
        EXPECT_TRUE(Compare(Storage[i], Storage[i - 1]));
    }
}

TEST_F(StorageTest, Clear)
{
    Storage.Clear();
    EXPECT_TRUE(Storage.IsEmpty());
}

TEST_F(StorageTest, Data)
{
    for (std::size_t i = 0; const auto& Current : Storage.Data())
    {
        EXPECT_EQ(Current, GetComponentAtReversed(i));
        ++i;
    }
}

TEST_F(StorageTest, Each)
{
    for (std::size_t i = 0; const auto [Entity, Component] : Storage.Each())
    {
        EXPECT_EQ(Storage.Get(Entity), Component);
        EXPECT_EQ(GetComponentAtReversed(i), Component);
        ++i;
    }
}

TEST_F(StorageTest, Get)
{
    for (std::size_t i = 0; const auto& Current : Storage)
    {
        EXPECT_EQ(Storage.Get(Current), GetComponentAtReversed(i));
        ++i;
    }
}

TEST_F(StorageTest, GetAsTuple)
{
    for (std::size_t i = 0; const auto& Current : Storage)
    {
        EXPECT_EQ(Storage.GetAsTuple(Current), std::forward_as_tuple(GetComponentAtReversed(i)));
        ++i;
    }
}

TEST_F(StorageTest, GetCapacity)
{
    EXPECT_EQ(Storage.GetCapacity(), ((IterationsCount - 1u) / TraitsType::PageSize + 1u) * TraitsType::PageSize);
}
