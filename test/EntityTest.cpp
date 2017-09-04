#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "entity/Entity.hpp"
#include "entity/World.hpp"

#include "util/Types.hpp"

#include <cstdint>

struct DummyComponent : public xanthus::Component
{
    bool flag = false;
};

using namespace xanthus::entity;

TEST_CASE("Dummy entity", "[empty]")
{
    Entity test;

    REQUIRE(false == test.IsValid());
}

TEST_CASE("Checking component in empty entity", "[empty]")
{
    World world;
    Entity test = world.CreateEntity();

    REQUIRE(false == test.HasComponent<DummyComponent>());
}

TEST_CASE("Add and delete component", "[general]")
{
    World world;
    Entity test = world.CreateEntity();

    test.AddComponent<DummyComponent>();
    REQUIRE(true == test.HasComponent<DummyComponent>());

    SECTION("delete component")
    {
        test.DeleteComponent<DummyComponent>();
        REQUIRE(false == test.HasComponent<DummyComponent>());
    }
}

TEST_CASE("Add component twice", "[general]")
{
    World world;
    Entity test = world.CreateEntity();

    test.AddComponent<DummyComponent>();
    REQUIRE(true == test.HasComponent<DummyComponent>());

    bool const initialFlag = test.GetComponent<DummyComponent>().flag;
    test.GetComponent<DummyComponent>().flag = !initialFlag;

    REQUIRE(initialFlag != test.GetComponent<DummyComponent>().flag);

    test.AddComponent<DummyComponent>();
    REQUIRE(true == test.HasComponent<DummyComponent>());

    REQUIRE(initialFlag == test.GetComponent<DummyComponent>().flag);

    SECTION("delete component")
    {
        test.DeleteComponent<DummyComponent>();
        REQUIRE(false == test.HasComponent<DummyComponent>());
    }
}

TEST_CASE("Delete component in empty entity", "[empty]")
{
    World world;
    Entity test = world.CreateEntity();

    test.DeleteComponent<DummyComponent>();

    REQUIRE(false == test.HasComponent<DummyComponent>());
}
