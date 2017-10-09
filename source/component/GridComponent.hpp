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
        : GridComponent(0, 0, 0)
    {

    }

    GridComponent(int16_t _x, int16_t _y, int16_t _z, int16_t _alt = 0)
        : x(_x), y(_y), z(_z), alt(_alt)
    {
        assert(0 == (x + y + z));
    }

    bool operator==(GridComponent const& other) const
    {
        return x == other.x
            && y == other.y
            && z == other.z
            && alt == other.alt;
    }

    bool operator!=(GridComponent const& other) const
    {
        return !operator==(other);
    }

    int16_t x;
    int16_t y;
    int16_t z;

    int16_t alt;
};

}
}

#endif // XANTHUS_COMPONENT_GRID_COMPONENT_HPP
