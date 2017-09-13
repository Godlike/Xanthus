#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "component/Flags.hpp"

using namespace xanthus::component;

struct ComponentA : public xanthus::Component {};
struct ComponentB : public xanthus::Component {};
struct ComponentC : public xanthus::Component {};

TEST_CASE("Dummy", "[empty]")
{
    Flags flags;

    REQUIRE(true == flags.None());
    REQUIRE(false == flags.Any());
    REQUIRE(false == flags.All());
}

TEST_CASE("Set and test", "[basic]")
{
    Flags flags;
    Flags inlineFlags;

    SECTION("one component")
    {
        flags.Set<ComponentA>();

        REQUIRE(true == flags.Any());

        REQUIRE(true == flags.Test<ComponentA>());
        REQUIRE(false == flags.Test<ComponentB>());
    }

    SECTION("multiple components")
    {
        flags.Set<ComponentA>();
        flags.Set<ComponentB>();

        REQUIRE(true == flags.Any());

        REQUIRE(true == flags.Test<ComponentA>());
        REQUIRE(true == flags.Test<ComponentB>());

        inlineFlags.Set<ComponentA, ComponentB>();

        REQUIRE(flags == inlineFlags);
    }

    SECTION("misc cases")
    {
        flags.Set<>();

        REQUIRE(true == flags.None());

        inlineFlags.Set<ComponentA, ComponentA>();

        REQUIRE(true == inlineFlags.Test<ComponentA>());
    }
}

TEST_CASE("Reset and test", "[basic]")
{
    Flags flags;

    flags.Set<ComponentA, ComponentB>();

    REQUIRE(true == flags.Any());

    REQUIRE(true == flags.Test<ComponentA>());
    REQUIRE(true == flags.Test<ComponentB>());

    SECTION("reset")
    {
        flags.Reset<ComponentA>();

        REQUIRE(true == flags.Any());

        REQUIRE(false == flags.Test<ComponentA>());
        REQUIRE(true == flags.Test<ComponentB>());

        flags.Reset<ComponentB>();

        REQUIRE(false == flags.Any());
        REQUIRE(true == flags.None());

        REQUIRE(false == flags.Test<ComponentA>());
        REQUIRE(false == flags.Test<ComponentB>());
    }

    SECTION("reset multiple")
    {
        flags.Reset<ComponentA, ComponentB>();

        REQUIRE(false == flags.Any());
        REQUIRE(true == flags.None());

        REQUIRE(false == flags.Test<ComponentA>());
        REQUIRE(false == flags.Test<ComponentB>());
    }

    SECTION("clear")
    {
        REQUIRE(true == flags.Any());

        flags.Clear();

        REQUIRE(true == flags.None());
        REQUIRE(false == flags.Any());
    }
}

TEST_CASE("Copy and assignment", "[basic]")
{
    Flags flagsA;
    flagsA.Set<ComponentA, ComponentB>();

    SECTION("copy")
    {
        Flags flagsB(flagsA);

        REQUIRE(flagsA == flagsB);
    }

    SECTION("assignment")
    {
        Flags flagsB;

        REQUIRE(flagsA != flagsB);

        flagsB = flagsA;

        REQUIRE(flagsA == flagsB);
    }
}

SCENARIO("Setting flags", "[general]")
{
    GIVEN("some flag is set")
    {
        Flags flags;
        flags.Set<ComponentA>();

        REQUIRE(true == flags.Test<ComponentA>());

        WHEN("it is set again")
        {
            flags.Set<ComponentA>();

            THEN("nothing happens")
            {
                REQUIRE(true == flags.Test<ComponentA>());
            }
        }
    }
}

SCENARIO("Resetting flags", "[general]")
{
    GIVEN("no flags are set")
    {
        Flags flags;

        REQUIRE(true == flags.None());

        WHEN("some flag is reset")
        {
            flags.Reset<ComponentA>();

            THEN("nothing happens")
            {
                REQUIRE(true == flags.None());
            }
        }
    }

    GIVEN("some flag is set")
    {
        Flags flags;
        flags.Set<ComponentA>();

        REQUIRE(true == flags.Test<ComponentA>());
        REQUIRE(false == flags.Test<ComponentB>());

        WHEN("another flag is reset")
        {
            flags.Reset<ComponentB>();

            THEN("nothing happens")
            {
                REQUIRE(true == flags.Test<ComponentA>());
                REQUIRE(false == flags.Test<ComponentB>());
            }
        }
    }

    GIVEN("two sets")
    {
        Flags flagsA;
        Flags flagsB;

        WHEN("same flags are set in different ways/order")
        {
            flagsA.Set<ComponentA>();
            flagsA.Set<ComponentB>();

            flagsB.Set<ComponentB, ComponentA>();

            THEN("they are equal in all ways")
            {
                REQUIRE(true == flagsA.Intersects(flagsB));
                REQUIRE(true == flagsA.IsSubset(flagsB));

                REQUIRE(flagsA == flagsB);
            }
        }
    }
}

SCENARIO("Comparing flags", "[general]")
{
    GIVEN("two sets")
    {
        Flags flagsA;
        Flags flagsB;

        WHEN("both are empty")
        {
            THEN("they are equal but are not intersecting")
            {
                REQUIRE(false == flagsA.Intersects(flagsB));
                REQUIRE(true == flagsA.IsSubset(flagsB));

                REQUIRE(flagsA == flagsB);
            }
        }

        WHEN("one of them is empty")
        {
            flagsA.Set<ComponentA>();

            THEN("they are different")
            {
                REQUIRE(flagsA != flagsB);
                REQUIRE(flagsB != flagsA);

                REQUIRE(false == flagsA.Intersects(flagsB));
                REQUIRE(false == flagsB.Intersects(flagsA));
            }
            AND_THEN("empty is a subset of non-empty")
            {
                REQUIRE(true == flagsB.IsSubset(flagsA));
            }
            AND_THEN("non-empty is not a subset of empty")
            {
                REQUIRE(false == flagsA.IsSubset(flagsB));
            }
        }

        WHEN("they have the same one component")
        {
            flagsA.Set<ComponentA>();
            flagsB.Set<ComponentA>();

            THEN("they are equal in all ways")
            {
                REQUIRE(true == flagsA.Intersects(flagsB));
                REQUIRE(true == flagsA.IsSubset(flagsB));

                REQUIRE(flagsA == flagsB);
            }
        }

        WHEN("they have the same multiple components")
        {
            flagsA.Set<ComponentA, ComponentB>();
            flagsB.Set<ComponentB, ComponentA>();

            THEN("they are equal in all ways")
            {
                REQUIRE(true == flagsA.Intersects(flagsB));
                REQUIRE(true == flagsA.IsSubset(flagsB));

                REQUIRE(flagsA == flagsB);
            }
        }

        WHEN("one has all components of the other")
        {
            flagsA.Set<ComponentA>();
            flagsB.Set<ComponentA, ComponentB>();

            THEN("they are not equal")
            {
                REQUIRE(flagsA != flagsB);
            }
            AND_THEN("smaller one is a subset of the bigger one")
            {
                REQUIRE(true == flagsA.Intersects(flagsB));
                REQUIRE(true == flagsA.IsSubset(flagsB));
            }
            AND_THEN("bigger one only intersects the smaller one")
            {
                REQUIRE(true == flagsB.Intersects(flagsA));
                REQUIRE(false == flagsB.IsSubset(flagsA));
            }
        }

        WHEN("their components intersect")
        {
            flagsA.Set<ComponentC, ComponentA>();
            flagsB.Set<ComponentA, ComponentB>();

            THEN("they are not equal")
            {
                REQUIRE(flagsA != flagsB);
            }
            AND_THEN("they are not a subset of each other")
            {
                REQUIRE(false == flagsA.IsSubset(flagsB));
                REQUIRE(false == flagsB.IsSubset(flagsA));
            }
            AND_THEN("they intersect each other")
            {
                REQUIRE(true == flagsA.Intersects(flagsB));
                REQUIRE(true == flagsB.Intersects(flagsA));
            }
        }

        WHEN("they have different components")
        {
            flagsA.Set<ComponentA>();
            flagsB.Set<ComponentB>();

            THEN("they are different in all ways")
            {
                REQUIRE(false == flagsA.Intersects(flagsB));
                REQUIRE(false == flagsB.Intersects(flagsA));

                REQUIRE(false == flagsA.IsSubset(flagsB));
                REQUIRE(false == flagsB.IsSubset(flagsA));

                REQUIRE(flagsA != flagsB);
                REQUIRE(flagsB != flagsA);
            }
        }
    }
}
