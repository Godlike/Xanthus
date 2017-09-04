#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "entity/EntityPool.hpp"

using namespace xanthus;
using namespace xanthus::entity;

TEST_CASE("Creating entities", "[general]")
{
    EntityPool pool;

    EntityId entity0 = pool.CreateEntity();
    REQUIRE(pool.EntityIsValid(entity0));
    REQUIRE(0 == entity0);

    EntityId entity1 = pool.CreateEntity();
    REQUIRE(pool.EntityIsValid(entity1));
    REQUIRE(1 == entity1);
}

TEST_CASE("Creating and deleting entities", "[general]")
{
    EntityPool pool;

    EntityId entity0 = pool.CreateEntity();
    pool.DeleteEntity(entity0);
    REQUIRE(!pool.EntityIsValid(entity0));

    EntityId entity1 = pool.CreateEntity();
    REQUIRE(pool.EntityIsValid(entity1));
    REQUIRE(0 == entity1);
}
