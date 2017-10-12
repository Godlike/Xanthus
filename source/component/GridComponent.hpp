#ifndef XANTHUS_COMPONENT_GRID_COMPONENT_HPP
#define XANTHUS_COMPONENT_GRID_COMPONENT_HPP

#include "util/Types.hpp"

#include <cassert>

namespace xanthus
{
namespace component
{

struct GridComponent : public Component
{
    GridComponent()
        : GridComponent(0, 0)
    {

    }

    GridComponent(int16_t _x, int16_t _y, int16_t _alt = 0)
        : x(_x), y(_y), alt(_alt)
    {

    }

    GridComponent(GridComponent const& other)
        : x(other.x)
        , y(other.y)
        , alt(other.alt)
    {

    }

    GridComponent& operator=(GridComponent const other)
    {
        x = other.x;
        y = other.y;
        alt = other.alt;

        return *this;
    }

    bool operator==(GridComponent const& other) const
    {
        return x == other.x
            && y == other.y
            && alt == other.alt;
    }

    bool operator!=(GridComponent const& other) const
    {
        return !operator==(other);
    }

    int16_t x;
    int16_t y;

    int16_t alt;
};

}
}

#endif // XANTHUS_COMPONENT_GRID_COMPONENT_HPP
