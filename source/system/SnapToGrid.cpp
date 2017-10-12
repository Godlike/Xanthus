#include "system/SnapToGrid.hpp"

#include "util/Config.hpp"

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

    double const altMultiplier = 1.0f;
    double const yOffset = 10.0f;
    double const spacing = 0.5f;
    double const sideMultiplier = util::Config::GridSide + spacing;

    for (entity::Entity& entity : entities)
    {
        PositionComponent& posComp = entity.GetComponent<PositionComponent>();
        GridComponent gridComp = entity.GetComponent<GridComponent>();

        posComp.position = {
            sideMultiplier * gridComp.x
            , altMultiplier * gridComp.alt + yOffset
            , sideMultiplier * gridComp.y
        };
    }
}

}
}
