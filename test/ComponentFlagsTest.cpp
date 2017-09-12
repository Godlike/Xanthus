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

    REQUIRE(true == flags.None());

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

TEST_CASE("Compare", "[basic]")
{
    Flags flagsA;
    Flags flagsB;

    SECTION("empty sets")
    {
        REQUIRE(true == flagsA.FullMatch(flagsB));
    }

    SECTION("equal sets")
    {
        flagsA.Set<ComponentA>();
        flagsB.Set<ComponentA>();

        REQUIRE(flagsA == flagsB);
        REQUIRE(flagsB == flagsA);

        REQUIRE(true == flagsA.FullMatch(flagsB));
        REQUIRE(true == flagsB.FullMatch(flagsA));

        REQUIRE(true == flagsA.PartialMatch(flagsB));
        REQUIRE(true == flagsB.PartialMatch(flagsA));
    }

    SECTION("full match")
    {
        flagsA.Set<ComponentA>();
        flagsB.Set<ComponentA, ComponentB>();

        REQUIRE(true == flagsA.FullMatch(flagsB));
        REQUIRE(false == flagsB.FullMatch(flagsA));

        REQUIRE(true == flagsA.PartialMatch(flagsB));
        REQUIRE(true == flagsB.PartialMatch(flagsA));
    }

    SECTION("partial match")
    {
        flagsA.Set<ComponentC, ComponentA>();
        flagsB.Set<ComponentA, ComponentB>();

        REQUIRE(false == flagsA.FullMatch(flagsB));
        REQUIRE(false == flagsB.FullMatch(flagsA));

        REQUIRE(true == flagsA.PartialMatch(flagsB));
        REQUIRE(true == flagsB.PartialMatch(flagsA));
    }

    SECTION("completely different sets")
    {
        flagsA.Set<ComponentA>();
        flagsB.Set<ComponentB>();

        REQUIRE(false == flagsA.FullMatch(flagsB));
        REQUIRE(false == flagsB.FullMatch(flagsA));

        REQUIRE(false == flagsA.PartialMatch(flagsB));
        REQUIRE(false == flagsB.PartialMatch(flagsA));

        REQUIRE(flagsA != flagsB);
        REQUIRE(flagsB != flagsA);
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
}
