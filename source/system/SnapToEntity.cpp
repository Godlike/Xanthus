#include "system/SnapToEntity.hpp"

namespace xanthus
{
namespace system
{

SnapToEntity::SnapToEntity(entity::World& world)
    : Skeleton<component::PositionComponent, component::EntitySnapComponent>(world)
{

}

void SnapToEntity::Update()
{
    using namespace component;

    entity::World::Entities entities = GetEntities();

    for (entity::Entity& entity : entities)
    {
        PositionComponent& posComp = entity.GetComponent<PositionComponent>();
        EntitySnapComponent const& snapComp = entity.GetComponent<EntitySnapComponent>();

        posComp.position = snapComp.entity.GetComponent<PositionComponent>().position + snapComp.offset;
    }
}

}
}
