#include "controller/Grid.hpp"

#include "component/LifetimeComponent.hpp"

#include <algorithm>

namespace xanthus
{
namespace controller
{

Grid::~Grid()
{
    for (auto & entities : m_grid)
    {
        for (auto & entity : entities.second)
        {
            if (entity.IsValid())
            {
                entity.AddComponent<component::LifetimeComponent>();
            }
        }
    }
}

entity::World::Entities Grid::Get(Coordinates coords) const
{
    auto it = m_grid.find(coords);

    if (it != m_grid.cend())
    {
        return it->second;
    }
    else
    {
        return entity::World::Entities();
    }
}

void Grid::Set(Coordinates coords, entity::World::Entities const& entities)
{
    if (m_grid.find(coords) != m_grid.end())
    {
        for (entity::Entity entity : m_grid[coords])
        {
            EntityId const entityId = entity.GetId();

            if (m_reverseGrid[entityId] == coords)
            {
                m_reverseGrid.erase(entityId);

                entity.DeleteComponent<component::GridComponent>();
            }
        }
    }

    m_grid[coords] = entities;

    for (entity::Entity entity : entities)
    {
        m_reverseGrid[entity.GetId()] = coords;

        entity.AddComponent<component::GridComponent>(coords);
    }
}

void Grid::Add(Coordinates coords, entity::Entity entity)
{
    m_grid[coords].push_back(entity);
    m_reverseGrid[entity.GetId()] = coords;

    entity.AddComponent<component::GridComponent>(coords);
}

void Grid::Remove(Coordinates coords, entity::Entity entity)
{
    auto gridIterator = m_grid.find(coords);

    if (gridIterator != m_grid.end())
    {
        {
            EntityId const entityId = entity.GetId();

            if (m_reverseGrid[entityId] == coords)
            {
                m_reverseGrid.erase(entityId);

                entity.DeleteComponent<component::GridComponent>();
            }
        }

        entity::World::Entities& entities = gridIterator->second;

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

}
}
