#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "util/TemplateCounter.hpp"

class CounterGroupOne {};
class CounterGroupTwo {};

class Dummy0 {};
class Dummy1 {};
class Dummy2 {};

SCENARIO("Template instantiations can be counted", "[general]")
{
    using namespace xanthus::util;

    GIVEN("one counter")
    {
        WHEN("several classes are instantiated with a counter")
        {
            TemplateCounter<CounterGroupOne>::GetId<Dummy0>();
            TemplateCounter<CounterGroupOne>::GetId<Dummy1>();

            THEN("their ids are sequential")
            {
                REQUIRE(0 == TemplateCounter<CounterGroupOne>::GetId<Dummy0>());
                REQUIRE(1 == TemplateCounter<CounterGroupOne>::GetId<Dummy1>());
            }
        }
    }

    GIVEN("two counters")
    {
        WHEN("class is instantiated with different counters")
        {
            TemplateCounter<CounterGroupOne>::GetId<Dummy0>();
            TemplateCounter<CounterGroupTwo>::GetId<Dummy0>();

            THEN("their ids are separate between counters and can be equal")
            {
                REQUIRE(TemplateCounter<CounterGroupTwo>::GetId<Dummy0>() == TemplateCounter<CounterGroupOne>::GetId<Dummy0>());
            }
        }

        WHEN("several classes are instantiated with different counters")
        {
            TemplateCounter<CounterGroupOne>::GetId<Dummy1>();
            TemplateCounter<CounterGroupOne>::GetId<Dummy2>();

            TemplateCounter<CounterGroupTwo>::GetId<Dummy2>();
            TemplateCounter<CounterGroupTwo>::GetId<Dummy1>();

            THEN("their ids between counters may differ based on the order of instantiation")
            {
                REQUIRE(TemplateCounter<CounterGroupTwo>::GetId<Dummy2>() != TemplateCounter<CounterGroupOne>::GetId<Dummy2>());
                REQUIRE(TemplateCounter<CounterGroupTwo>::GetId<Dummy1>() != TemplateCounter<CounterGroupOne>::GetId<Dummy1>());
            }
        }
    }
}
