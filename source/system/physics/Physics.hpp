#ifndef XANTHUS_SYSTEM_PHYSICS_PHYSICS_HPP
#define XANTHUS_SYSTEM_PHYSICS_PHYSICS_HPP

#include "WorldTime.hpp"

#include "entity/World.hpp"

#include "component/Flags.hpp"
#include "component/PositionComponent.hpp"
#include "component/PhysicsComponent.hpp"

#include "system/Skeleton.hpp"
#include "system/physics/PhysicsThread.hpp"
#include "system/physics/SpawnInfo.hpp"

#include "util/Config.hpp"

#include <list>
#include <memory>
#include <vector>

namespace xanthus
{
namespace system
{
namespace physics
{

class Physics : public Skeleton<component::PositionComponent, component::PhysicsComponent>
{
public:
    Physics(entity::World& world
        , WorldTime& worldTime
    );
    ~Physics();

    void Init();
    void Update();

    BodyHandle* SpawnBody(SpawnInfo const& info);
    void DeleteBody(BodyHandle const* pHandle);

private:
    PhysicsThread m_physicsThread;
    PhysicsThread::ThreadIndex m_threadId;
};

}
}
}

#endif // XANTHUS_SYSTEM_PHYSICS_PHYSICS_HPP
