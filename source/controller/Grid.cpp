#include "controller/Grid.hpp"

#include "component/LifetimeComponent.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <algorithm>

namespace xanthus
{
namespace controller
{

Grid::~Grid()
{
    for (auto & tile : m_grid)
    {
        for (auto & entity : tile.second.entities)
        {
            if (entity.IsValid())
            {
                entity.AddComponent<component::LifetimeComponent>();
            }
        }
    }
}

void Grid::CreateTile(Coordinates coords)
{
    assert(m_coords.empty());

    m_grid[coords].coordinates = coords;
}

void Grid::BuildConnections()
{
    assert(m_coords.empty());

    std::transform(m_grid.begin(), m_grid.end(), std::inserter(m_coords, m_coords.end()),
        [](auto gridIt) -> Coordinates
        {
            return gridIt.first;
        }
    );

    for (Coordinates const& coords : m_coords)
    {
        Tile& tile = m_grid[coords];
        glm::vec3 origin = {coords.x, coords.y, static_cast<float>(coords.alt) * util::Config::GridAltFactor};

        Coordinates adjacents[] = {
            Coordinates(coords.x - 1, coords.y - 1)
            , Coordinates(coords.x + 0, coords.y - 1)
            , Coordinates(coords.x + 1, coords.y - 1)
            , Coordinates(coords.x - 1, coords.y + 0)
            , Coordinates(coords.x + 0, coords.y + 0)
            , Coordinates(coords.x + 1, coords.y + 0)
            , Coordinates(coords.x - 1, coords.y + 1)
            , Coordinates(coords.x + 0, coords.y + 1)
            , Coordinates(coords.x + 1, coords.y + 1)
        };

        for (Coordinates & neighbour : adjacents)
        {
            auto range = m_coords.equal_range(neighbour);

            for (auto it = range.first; it != range.second; ++it)
            {
                float distance = glm::distance2(origin, glm::vec3{it->x, it->y, static_cast<float>(it->alt) * util::Config::GridAltFactor});

                if (distance < 2.0f)
                {
                    Tile& targetTile = m_grid[*it];
                    tile.neighbours.push_back(&targetTile);
                    targetTile.neighbours.push_back(&tile);
                }
            }
        }
    }
}

entity::World::Entities Grid::Get(Coordinates coords) const
{
    assert(m_grid.find(coords) != m_grid.end());

    return m_grid.find(coords)->second.entities;
}

void Grid::Set(Coordinates coords, entity::World::Entities const& entities)
{
    assert(m_grid.find(coords) != m_grid.end());

    for (entity::Entity entity : m_grid[coords].entities)
    {
        entity.DeleteComponent<component::GridComponent>();
    }

    m_grid[coords].entities = entities;

    for (entity::Entity entity : entities)
    {
        entity.AddComponent<component::GridComponent>(coords);
    }
}

void Grid::Add(Coordinates coords, entity::Entity entity)
{
    assert(m_grid.find(coords) != m_grid.end());

    m_grid[coords].entities.push_back(entity);

    entity.AddComponent<component::GridComponent>(coords);
}

void Grid::Remove(Coordinates coords, entity::Entity entity)
{
    assert(m_grid.find(coords) != m_grid.end());

    entity.DeleteComponent<component::GridComponent>();

    auto gridIterator = m_grid.find(coords);

    if (gridIterator != m_grid.end())
    {
        entity::World::Entities& entities = gridIterator->second.entities;

        auto entityIterator = std::find_if(entities.begin(), entities.end(), [&entity](auto const& e) -> bool
        {
            return e == entity;
        });

        if (entityIterator != entities.end())
        {
            entities.erase(entityIterator);
        }
    }
}

void Grid::Remove(entity::Entity entity)
{
    if (entity.HasComponent<component::GridComponent>())
    {
        Remove(entity.GetComponent<component::GridComponent>(), entity);
    }
}

Grid::Tile const& Grid::GetTile(Coordinates coords) const
{
    return m_grid.at(coords);
}

}
}
