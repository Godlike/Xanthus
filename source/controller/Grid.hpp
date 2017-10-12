#ifndef XANTHUS_CONTROLLER_GRID_HPP
#define XANTHUS_CONTROLLER_GRID_HPP

#include "entity/Entity.hpp"
#include "entity/World.hpp"

#include "component/GridComponent.hpp"

#include "util/Types.hpp"

#include <cstddef>
#include <functional>
#include <unordered_map>

namespace xanthus
{
namespace controller
{

class Grid
{
public:
    using Coordinates = component::GridComponent;

    Grid() = default;

    Grid(Grid const& other) = delete;
    Grid& operator=(Grid const& other) = delete;

    ~Grid();

    entity::World::Entities Get(Coordinates coords) const;
    void Set(Coordinates coords, entity::World::Entities const& entities);

    void Add(Coordinates coords, entity::Entity entity);
    void Remove(Coordinates coords, entity::Entity entity);

private:
    struct CoordinateHasher
    {
        std::size_t operator()(Coordinates const& coords) const
        {
            return std::hash<uint64_t>{}(
                (uint64_t(coords.x) << 32)
                | (uint64_t(coords.y) << 16)
                | uint64_t(coords.alt)
            );
        }
    };

    std::unordered_map<Coordinates, entity::World::Entities, CoordinateHasher> m_grid;
    std::unordered_map<EntityId, Coordinates> m_reverseGrid;
};

}
}

#endif // XANTHUS_CONTROLLER_GRID_HPP
