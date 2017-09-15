#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "entity/EntityPool.hpp"

#include "component/Flags.hpp"
#include "util/Types.hpp"

#include <algorithm>
#include <array>
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

    SECTION("individual")
    {
        EntityId entity0 = pool.CreateEntity();
        REQUIRE(true == pool.EntityIsValid(entity0));
        REQUIRE(0 == entity0);

        EntityId entity1 = pool.CreateEntity();
        REQUIRE(true == pool.EntityIsValid(entity1));
        REQUIRE(1 == entity1);
    }

    SECTION("batch")
    {
        const std::size_t entityCount = 8;
        EntityPool::EntityCollection entities = pool.CreateEntities(entityCount);
        REQUIRE(entityCount == entities.size());

        const EntityId minId = entities.front();
        const EntityId maxId = entities.back();

        REQUIRE(true == pool.EntityIsValid(minId));
        REQUIRE(true == pool.EntityIsValid(entities[entityCount / 2]));
        REQUIRE(true == pool.EntityIsValid(maxId));

        REQUIRE(false == pool.EntityIsValid(minId - 1));
        REQUIRE(false == pool.EntityIsValid(maxId + 1));
    }

    SECTION("mixed")
    {
        const std::size_t entityCount = 8;

        EntityId entityMin = pool.CreateEntity();
        EntityPool::EntityCollection entities = pool.CreateEntities(entityCount);
        EntityId entityMax = pool.CreateEntity();

        REQUIRE(entityMin == entities.front() - 1);
        REQUIRE(entityMax == entities.back() + 1);
    }
}

TEST_CASE("Deleting entities", "[basic]")
{
    EntityPool pool;

    SECTION("individual")
    {
        EntityId entity0 = pool.CreateEntity();
        pool.DeleteEntity(entity0);
        REQUIRE(false == pool.EntityIsValid(entity0));

        EntityId entity1 = pool.CreateEntity();
        REQUIRE(entity0 == entity1);
        REQUIRE(true == pool.EntityIsValid(entity1));
    }

    SECTION("batch")
    {
        const std::size_t entityCount = 8;
        EntityPool::EntityCollection entities = pool.CreateEntities(entityCount);
        pool.DeleteEntities(entities);

        REQUIRE(false == pool.EntityIsValid(entities.front()));
        REQUIRE(false == pool.EntityIsValid(entities[entityCount / 2]));
        REQUIRE(false == pool.EntityIsValid(entities.back()));
    }

    SECTION("mixed")
    {
        EntityId entity0 = pool.CreateEntity();
        pool.DeleteEntity(entity0); // invalidates entity0

        REQUIRE(false == pool.EntityIsValid(entity0));

        // Newly created entities shall first reuse invalidated ones
        const std::size_t entityCount = 8;
        EntityPool::EntityCollection entities = pool.CreateEntities(entityCount);

        REQUIRE(entity0 == entities.front());
        REQUIRE(true == pool.EntityIsValid(entities.front()));

        pool.DeleteEntities(entities);  // invalidates entities

        // Newly created entity shall reuse invalidated one
        EntityId entity1 = pool.CreateEntity();
        REQUIRE(entity0 == entity1);
        REQUIRE(true == pool.EntityIsValid(entity1));
    }
}

SCENARIO("Matching entities", "[general]")
{
    EntityPool pool;

    GIVEN("empty entities")
    {
        EntityPool::EntityCollection entities = pool.CreateEntities(8);

        WHEN("match query is empty")
        {
            EntityPool::EntityCollection match = pool.MatchEntities(Flags());

            THEN("everything is matched")
            {
                REQUIRE(entities.size() == match.size());
            }
        }

        WHEN("match query is not empty")
        {
            Flags query;
            query.Set(0);

            EntityPool::EntityCollection match = pool.MatchEntities(query);

            THEN("nothing is matched")
            {
                REQUIRE(0 == match.size());
            }
        }
    }

    GIVEN("preset entities")
    {
        EntityPool::EntityCollection entities = pool.CreateEntities(8);
        /**
         *  Component layout:
         *  - 0: 0
         *  - 1: empty
         *  - 2: 1
         *  - 3: 0, 2
         *  - 4: 1, 2
         *  - 5: empty
         *  - 6: 2
         *  - 7: 0, 1, 2
         */

        pool.EntityAddComponent(entities[0], new DummyComponent(), 0);

        pool.EntityAddComponent(entities[2], new DummyComponent(), 2);

        pool.EntityAddComponent(entities[3], new DummyComponent(), 0);
        pool.EntityAddComponent(entities[3], new DummyComponent(), 2);

        pool.EntityAddComponent(entities[4], new DummyComponent(), 1);
        pool.EntityAddComponent(entities[4], new DummyComponent(), 2);

        pool.EntityAddComponent(entities[6], new DummyComponent(), 2);

        pool.EntityAddComponent(entities[7], new DummyComponent(), 0);
        pool.EntityAddComponent(entities[7], new DummyComponent(), 1);
        pool.EntityAddComponent(entities[7], new DummyComponent(), 2);


        WHEN("empty query is used")
        {
            EntityPool::EntityCollection matched = pool.MatchEntities(Flags());

            THEN("all entries are matched")
            {
                CAPTURE(matched);

                REQUIRE(entities.size() == matched.size());

                for (std::size_t i = 0; i < entities.size(); ++i)
                {
                    REQUIRE(entities[i] == matched[i]);
                }
            }
        }
        AND_WHEN("one component is queried")
        {
            Flags query;
            query.Set(0);

            EntityPool::EntityCollection matched = pool.MatchEntities(query);

            THEN("entries with this component are matched")
            {
                CAPTURE(matched);

                REQUIRE(3 == matched.size());

                REQUIRE(0 == matched[0]);
                REQUIRE(3 == matched[1]);
                REQUIRE(7 == matched[2]);
            }
        }
        AND_WHEN("multiple components are queried")
        {
            Flags query;
            query.Set(1);
            query.Set(2);

            EntityPool::EntityCollection matched = pool.MatchEntities(query);

            THEN("entries with these components are matched")
            {
                CAPTURE(matched);

                REQUIRE(2 == matched.size());

                REQUIRE(4 == matched[0]);
                REQUIRE(7 == matched[1]);
            }
        }
        AND_WHEN("unknown component is queried")
        {
            Flags query;
            query.Set(util::Config::MaxComponentCount - 1);

            EntityPool::EntityCollection matched = pool.MatchEntities(query);

            THEN("nothing is matched")
            {
                CAPTURE(matched);

                REQUIRE(true == matched.empty());
            }
        }
    }

    GIVEN("random entities")
    {
        EntityPool::EntityCollection entities = pool.CreateEntities(128);

        const std::size_t componentCount = 8;
        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::array<std::size_t, 8> components;
        std::array<EntityPool::EntityCollection, componentCount> expectedEntities;

        std::iota(components.begin(), components.end(), 0);

        // Generate entities and prepare expected results
        {
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

        WHEN("one component is queried")
        {
            for (std::size_t const& cId : components)
            {
                Flags query;
                query.Set(cId);

                EntityPool::EntityCollection match = pool.MatchEntities(query);
                EntityPool::EntityCollection const& expected = expectedEntities[cId];

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

        WHEN("random components are queried")
        {
            std::uniform_int_distribution<> entityIndex(0, entities.size() - 1);

            uint8_t const minTests = 8;
            uint8_t const maxTests = entities.size();
            uint8_t const testCount = std::uniform_int_distribution<>(minTests, maxTests)(randEngine);

            for (uint8_t i = 0; i < testCount; ++i)
            {
                Flags query;
                std::vector<std::size_t> queryComponents;
                EntityPool::EntityCollection expected;

                {
                    EntityId entityId = entities[entityIndex(randEngine)];

                    // Take all components of a random entity
                    for (std::size_t const& cId : components)
                    {
                        EntityPool::EntityCollection const& validEntities = expectedEntities[cId];
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
                            EntityPool::EntityCollection const& validEntities = expectedEntities[cId];
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

                EntityPool::EntityCollection match = pool.MatchEntities(query);

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

SCENARIO("Component manipulations", "[general]")
{
    EntityPool pool;
    EntityId entity = pool.CreateEntity();

    GIVEN("one component")
    {
        const std::size_t componentId = 0;
        Component* pComp = new DummyComponent();

        REQUIRE(false == pool.EntityHasComponent(entity, componentId));

        WHEN("component is added")
        {
            pool.EntityAddComponent(entity, pComp, componentId);

            THEN("it is visible")
            {
                REQUIRE(true == pool.EntityHasComponent(entity, componentId));
            }
            AND_THEN("it points to the created component")
            {
                Component& c = pool.EntityGetComponent(entity, componentId);
                REQUIRE(&c == pComp);
            }
        }
        AND_WHEN("component is deleted")
        {
            pool.EntityAddComponent(entity, pComp, componentId);
            pool.EntityDeleteComponent(entity, componentId);

            THEN("is is not visible anymore")
            {
                REQUIRE(false == pool.EntityHasComponent(entity, componentId));
            }
        }
        AND_WHEN("component is added twice")
        {
            pool.EntityAddComponent(entity, pComp, componentId);
            pool.EntityAddComponent(entity, new DummyComponent(), componentId);

            THEN("it is visible")
            {
                REQUIRE(true == pool.EntityHasComponent(entity, componentId));
            }
            AND_THEN("it no longer point to the original component")
            {
                Component& c = pool.EntityGetComponent(entity, componentId);
                REQUIRE(&c != pComp);
            }
        }
    }

    GIVEN("multiple components")
    {
        pool.EntityAddComponent(entity, new DummyComponent(), 0);
        pool.EntityAddComponent(entity, new DummyComponent(), 1);
        pool.EntityAddComponent(entity, new DummyComponent(), 2);
        pool.EntityAddComponent(entity, new DummyComponent(), 3);

        WHEN("some components are deleted")
        {
            pool.EntityDeleteComponent(entity, 0);
            pool.EntityDeleteComponent(entity, 2);

            THEN("they are not visible")
            {
                REQUIRE(false == pool.EntityHasComponent(entity, 0));
                REQUIRE(false == pool.EntityHasComponent(entity, 2));
            }
            AND_THEN("the rest are visible")
            {
                REQUIRE(true == pool.EntityHasComponent(entity, 1));
                REQUIRE(true == pool.EntityHasComponent(entity, 3));
            }
        }

        WHEN("components are deleted in bulk")
        {
            pool.EntityDeleteComponents(entity);

            THEN("no components are visible")
            {
                REQUIRE(false == pool.EntityHasComponent(entity, 0));
                REQUIRE(false == pool.EntityHasComponent(entity, 1));
                REQUIRE(false == pool.EntityHasComponent(entity, 2));
                REQUIRE(false == pool.EntityHasComponent(entity, 3));
            }
        }
    }
}
