#ifndef XANTHUS_COMPONENT_GRID_COMPONENT_HPP
#define XANTHUS_COMPONENT_GRID_COMPONENT_HPP

#include "util/Config.hpp"

#include <sleipnir/ecs/Types.hpp>

#include <glm/glm.hpp>

namespace xanthus
{
namespace component
{

struct GridComponent : public sleipnir::ecs::Component
{
    struct Hasher
    {
        std::size_t operator()(GridComponent const& coords) const
        {
            return std::hash<uint64_t>{}(
                (uint64_t(coords.x) << 32)
                | (uint64_t(coords.y) << 16)
                | uint64_t(coords.alt)
            );
        }
    };

    struct CoordinateHasher
    {
        std::size_t operator()(GridComponent const& coords) const
        {
            return std::hash<uint64_t>{}(
                (uint64_t(coords.x) << 32)
                | (uint64_t(coords.y) << 16)
            );
        }
    };

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

    operator glm::vec3() const
    {
        static double const altMultiplier = 1.0f;
        static double const spacing = 0.5f;
        static double const sideMultiplier = util::Config::GridSide + spacing;

        return glm::vec3{
            sideMultiplier * x
            , altMultiplier * alt
            , sideMultiplier * y
        };
    }

    int16_t x;
    int16_t y;

    int16_t alt;
};

}
}

#endif // XANTHUS_COMPONENT_GRID_COMPONENT_HPP
