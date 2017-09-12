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

        flags.Set<ComponentA, ComponentA>();

        REQUIRE(true == flags.Test<ComponentA>());
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
        REQUIRE(true == flagsA.FullMatch(flagsB));
    }

    SECTION("assignment")
    {
        Flags flagsB;

        REQUIRE(false == flagsA.PartialMatch(flagsB));

        flagsB = flagsA;

        REQUIRE(true == flagsA.FullMatch(flagsB));
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
                REQUIRE(false == flags.Test<ComponentB>());
                REQUIRE(true == flags.Test<ComponentA>());
            }
        }
    }

    GIVEN("two sets")
    {
        Flags flagsA;
        Flags flagsB;

        WHEN("same flags are set in different ways and order")
        {
            flagsA.Set<ComponentA>();
            flagsA.Set<ComponentB>();

            flagsB.Set<ComponentB, ComponentA>();

            THEN("they are equal in all ways")
            {
                REQUIRE(true == flagsA.PartialMatch(flagsB));
                REQUIRE(true == flagsA.FullMatch(flagsB));

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
            THEN("they are equal in all ways")
            {
                REQUIRE(true == flagsA.PartialMatch(flagsB));
                REQUIRE(true == flagsA.FullMatch(flagsB));

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
            }
            AND_THEN("they are matching from empty POV")
            {
                REQUIRE(true == flagsB.PartialMatch(flagsA));
                REQUIRE(true == flagsB.FullMatch(flagsA));
            }
            AND_THEN("they are not matching from non-empty POV")
            {
                REQUIRE(false == flagsA.PartialMatch(flagsB));
                REQUIRE(false == flagsA.FullMatch(flagsB));
            }
        }

        WHEN("they have the same component")
        {
            flagsA.Set<ComponentA>();
            flagsB.Set<ComponentA>();

            THEN("they are equal in all ways")
            {
                REQUIRE(true == flagsA.PartialMatch(flagsB));
                REQUIRE(true == flagsA.FullMatch(flagsB));

                REQUIRE(flagsA == flagsB);
            }
        }

        WHEN("they have the same components")
        {
            flagsA.Set<ComponentA, ComponentB>();
            flagsB.Set<ComponentB, ComponentA>();

            THEN("they are equal in all ways")
            {
                REQUIRE(true == flagsA.PartialMatch(flagsB));
                REQUIRE(true == flagsA.FullMatch(flagsB));

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
            AND_THEN("smaller one is matched by the bigger one")
            {
                REQUIRE(true == flagsA.PartialMatch(flagsB));
                REQUIRE(true == flagsA.FullMatch(flagsB));
            }
            AND_THEN("bigger one is only partially matched by the smaller one")
            {
                REQUIRE(true == flagsB.PartialMatch(flagsA));
                REQUIRE(false == flagsB.FullMatch(flagsA));
            }
        }

        WHEN("their flags are intersecting")
        {
            flagsA.Set<ComponentC, ComponentA>();
            flagsB.Set<ComponentA, ComponentB>();

            THEN("they are not equal")
            {
                REQUIRE(flagsA != flagsB);
            }
            AND_THEN("they don't fully match either way")
            {
                REQUIRE(false == flagsA.FullMatch(flagsB));
                REQUIRE(false == flagsB.FullMatch(flagsA));
            }
            AND_THEN("they partially match each other")
            {
                REQUIRE(true == flagsA.PartialMatch(flagsB));
                REQUIRE(true == flagsB.PartialMatch(flagsA));
            }
        }

        WHEN("they are different")
        {
            flagsA.Set<ComponentA>();
            flagsB.Set<ComponentB>();

            THEN("they are different in all ways")
            {
                REQUIRE(false == flagsA.PartialMatch(flagsB));
                REQUIRE(false == flagsB.PartialMatch(flagsA));

                REQUIRE(false == flagsA.FullMatch(flagsB));
                REQUIRE(false == flagsB.FullMatch(flagsA));

                REQUIRE(flagsA != flagsB);
                REQUIRE(flagsB != flagsA);
            }
        }
    }
}
