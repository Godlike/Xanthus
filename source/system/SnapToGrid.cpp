#include "system/SnapToGrid.hpp"

namespace xanthus
{
namespace system
{

SnapToGrid::SnapToGrid(entity::World& world)
    : Skeleton<component::PositionComponent, component::GridComponent>(world)
{

}

void SnapToGrid::Update()
{
    using namespace component;

    entity::World::Entities entities = GetEntities();

    for (entity::Entity& entity : entities)
    {
        PositionComponent& posComp = entity.GetComponent<PositionComponent>();
        GridComponent const& gridComp = entity.GetComponent<GridComponent>();

        posComp.position = static_cast<glm::vec3>(gridComp);
    }
}

}
}
