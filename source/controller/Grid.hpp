#ifndef XANTHUS_CONTROLLER_GRID_HPP
#define XANTHUS_CONTROLLER_GRID_HPP

#include "entity/Entity.hpp"
#include "entity/World.hpp"

#include "component/GridComponent.hpp"

#include "util/Types.hpp"

#include <cstddef>
#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace xanthus
{
namespace controller
{

class Grid
{
public:
    using Coordinates = component::GridComponent;

    struct Tile
    {
        Coordinates coordinates;
        entity::World::Entities entities;

        std::vector<Tile const*> neighbours;
    };

    Grid() = default;

    Grid(Grid const& other) = delete;
    Grid& operator=(Grid const& other) = delete;

    ~Grid();

    void CreateTile(Coordinates coords);
    void BuildConnections();

    entity::World::Entities Get(Coordinates coords) const;
    void Set(Coordinates coords, entity::World::Entities const& entities);

    void Add(Coordinates coords, entity::Entity entity);
    void Remove(Coordinates coords, entity::Entity entity);
    void Remove(entity::Entity entity);

    Tile const& GetTile(Coordinates coords) const;

private:
    std::unordered_map<Coordinates, Tile, Coordinates::Hasher> m_grid;
    std::unordered_multiset<Coordinates, Coordinates::CoordinateHasher> m_coords;
};

}
}

#endif // XANTHUS_CONTROLLER_GRID_HPP
