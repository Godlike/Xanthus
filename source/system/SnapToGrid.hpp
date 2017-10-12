#ifndef XANTHUS_SYSTEM_SNAP_TO_GRID_HPP
#define XANTHUS_SYSTEM_SNAP_TO_GRID_HPP

#include "entity/World.hpp"

#include "component/GridComponent.hpp"
#include "component/PositionComponent.hpp"

#include "system/Skeleton.hpp"

namespace xanthus
{
namespace system
{

class SnapToGrid
    : public Skeleton<component::PositionComponent, component::GridComponent>
{
public:
    SnapToGrid(entity::World& world);
    ~SnapToGrid() = default;

    void Update();
};

}
}

#endif // XANTHUS_SYSTEM_SNAP_TO_GRID_HPP
