#ifndef XANTHUS_SYSTEM_SNAP_TO_ENTITY_HPP
#define XANTHUS_SYSTEM_SNAP_TO_ENTITY_HPP

#include "entity/World.hpp"

#include "component/EntitySnapComponent.hpp"
#include "component/PositionComponent.hpp"

#include "system/Skeleton.hpp"

namespace xanthus
{
namespace system
{

class SnapToEntity
    : public Skeleton<component::PositionComponent, component::EntitySnapComponent>
{
public:
    SnapToEntity(entity::World& world);
    ~SnapToEntity() = default;

    void Update();
};

}
}

#endif // XANTHUS_SYSTEM_SNAP_TO_ENTITY_HPP
