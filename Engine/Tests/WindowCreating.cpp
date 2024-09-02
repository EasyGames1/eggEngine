#include <ECS/Systems/Renderer/Renderer.h>
#include <gtest/gtest.h>

TEST(Creating, Window)
{
    egg::Systems::Renderer Renderer;

    EXPECT_NO_THROW(Renderer.Initialize());
}
