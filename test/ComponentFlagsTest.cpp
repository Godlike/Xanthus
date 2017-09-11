#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "component/Flags.hpp"

using namespace xanthus::component;

struct ComponentA : public xanthus::Component {};
struct ComponentB : public xanthus::Component {};

TEST_CASE("Dummy", "[empty]")
{
    Flags flags;

    REQUIRE(true == flags.None());
}

TEST_CASE("Set", "[basic]")
{
    Flags flags;

    SECTION("one component")
    {
        flags.Set<ComponentA>();

        REQUIRE(true == flags.Any());

        REQUIRE(true == flags.Test<ComponentA>());
        REQUIRE(false == flags.Test<ComponentB>());
    }

    SECTION("multiple components separately")
    {
        flags.Set<ComponentA>();
        flags.Set<ComponentB>();

        REQUIRE(true == flags.Any());

        REQUIRE(true == flags.Test<ComponentA>());
        REQUIRE(true == flags.Test<ComponentB>());
    }

    SECTION("multiple components inline")
    {
        flags.Set<ComponentA, ComponentB>();

        REQUIRE(true == flags.Any());

        REQUIRE(true == flags.Test<ComponentA>());
        REQUIRE(true == flags.Test<ComponentB>());
    }

    SECTION("setting multiple times")
    {
        flags.Set<ComponentA>();

        REQUIRE(true == flags.Test<ComponentA>());

        flags.Set<ComponentA>();

        REQUIRE(true == flags.Test<ComponentA>());
    }
}

TEST_CASE("Reset", "[basic]")
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

        REQUIRE(true == flags.None());

        REQUIRE(false == flags.Test<ComponentA>());
        REQUIRE(false == flags.Test<ComponentB>());
    }

    SECTION("clear")
    {
        REQUIRE(true == flags.Any());

        flags.Clear();

        REQUIRE(true == flags.None());
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

    SECTION("full match")
    {
        flagsA.Set<ComponentA>();
        flagsB.Set<ComponentA>();

        REQUIRE(true == flagsA.FullMatch(flagsB));
    }

    SECTION("partial match")
    {
        flagsA.Set<ComponentA>();
        flagsB.Set<ComponentA, ComponentB>();

        REQUIRE(true == flagsA.PartialMatch(flagsB));
        REQUIRE(true == flagsB.PartialMatch(flagsA));
    }

    SECTION("no match")
    {
        flagsA.Set<ComponentA>();
        flagsB.Set<ComponentB>();

        REQUIRE(false == flagsA.PartialMatch(flagsB));
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
