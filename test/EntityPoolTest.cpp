#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "entity/EntityPool.hpp"

#include "component/Flags.hpp"
#include "util/Types.hpp"

#include <random>

using namespace xanthus;
using namespace xanthus::entity;
using namespace xanthus::component;

namespace
{
    struct DummyComponent : public xanthus::Component {};
}

TEST_CASE("Creating entities", "[basic]")
{
    EntityPool pool;

    EntityId entity0 = pool.CreateEntity();
    REQUIRE(pool.EntityIsValid(entity0));
    REQUIRE(0 == entity0);

    EntityId entity1 = pool.CreateEntity();
    REQUIRE(pool.EntityIsValid(entity1));
    REQUIRE(1 == entity1);
}

TEST_CASE("Creating and deleting entities", "[basic]")
{
    EntityPool pool;

    EntityId entity0 = pool.CreateEntity();
    pool.DeleteEntity(entity0);
    REQUIRE(false == pool.EntityIsValid(entity0));

    EntityId entity1 = pool.CreateEntity();
    REQUIRE(pool.EntityIsValid(entity1));
    REQUIRE(0 == entity1);
}

SCENARIO("Matching entities", "[general]")
{
    EntityPool pool;

    EntityPool::ComponentCollection entities = pool.CreateEntities(128);

    GIVEN("empty entities")
    {
        WHEN("match query is empty")
        {
            EntityPool::ComponentCollection match = pool.MatchEntities(Flags());

            THEN("everything is matched")
            {
                REQUIRE(entities.size() == match.size());
            }
        }

        WHEN("match query is not empty")
        {
            Flags query;
            query.Set(0);

            EntityPool::ComponentCollection match = pool.MatchEntities(query);

            THEN("nothing is matched")
            {
                REQUIRE(0 == match.size());
            }
        }
    }

    GIVEN("random entities")
    {
        std::size_t components[] = {0, 1, 2, 3, 4, 5, 6, 7};
        EntityPool::ComponentCollection expectedEntities[8];

        {
            std::random_device rd;
            std::mt19937 randEngine(rd());
            std::uniform_real_distribution<> componentFrequency(0.1, 0.9);

            for (std::size_t const& cId : components)
            {
                std::bernoulli_distribution validComponent(componentFrequency(randEngine));

                for (EntityId const& eId : entities)
                {
                    if (validComponent(randEngine))
                    {
                        pool.EntityAddComponent(eId, new DummyComponent(), cId);
                        expectedEntities[cId].push_back(eId);
                    }
                }

                INFO("Entities with component " << cId);
                CAPTURE(expectedEntities[cId]);
                CHECK(true);
            }
        }

        WHEN("single matching queries")
        {
            for (std::size_t const& cId : components)
            {
                Flags query;
                query.Set(cId);

                EntityPool::ComponentCollection match = pool.MatchEntities(query);
                EntityPool::ComponentCollection const& expected = expectedEntities[cId];

                INFO("Checking component " << cId);

                uint32_t matches = 0;

                for (EntityId const& eId : expected)
                {
                    for (EntityId const& mId : match)
                    {
                        if (eId == mId)
                        {
                            ++matches;
                            break;
                        }
                    }
                }

                {
                    CAPTURE(expected);
                    CAPTURE(match);

                    CHECK(true);
                }

                REQUIRE(expected.size() == matches);

                REQUIRE(expected.size() == match.size());
            }
        }

        WHEN("random matching queries")
        {
            std::random_device rd;
            std::mt19937 randEngine(rd());
            std::uniform_int_distribution<> entityIndex(0, entities.size() - 1);

            uint8_t const minTests = 8;
            uint8_t const maxTests = entities.size();
            uint8_t const testCount = std::uniform_int_distribution<>(minTests, maxTests)(randEngine);

            for (uint8_t i = 0; i < testCount; ++i)
            {
                Flags query;
                std::vector<std::size_t> queryComponents;
                EntityPool::ComponentCollection expected;

                {
                    EntityId entityId = entities[entityIndex(randEngine)];

                    // Take all components of a random entity
                    for (std::size_t const& cId : components)
                    {
                        EntityPool::ComponentCollection const& validEntities = expectedEntities[cId];
                        if (validEntities.cend() != std::find(validEntities.cbegin(), validEntities.cend(), entityId))
                        {
                            query.Set(cId);
                            queryComponents.push_back(cId);
                        }
                    }

                    // Find all entities that registered these components
                    for (EntityId const& eId : entities)
                    {
                        uint32_t count = 0;

                        for (std::size_t const& cId : queryComponents)
                        {
                            EntityPool::ComponentCollection const& validEntities = expectedEntities[cId];
                            if (validEntities.cend() != std::find(validEntities.cbegin(), validEntities.cend(), eId))
                            {
                                ++count;
                            }
                        }

                        if (queryComponents.size() == count)
                        {
                            expected.push_back(eId);
                        }
                    }

                    CAPTURE(queryComponents);
                    REQUIRE(true);
                }

                EntityPool::ComponentCollection match = pool.MatchEntities(query);

                uint32_t matches = 0;

                for (EntityId const& eId : expected)
                {
                    for (EntityId const& mId : match)
                    {
                        if (eId == mId)
                        {
                            ++matches;
                            break;
                        }
                    }
                }

                {
                    CAPTURE(expected);
                    CAPTURE(match);

                    CHECK(true);
                }

                REQUIRE(expected.size() == matches);
                REQUIRE(expected.size() == match.size());
            }
        }
    }
}
