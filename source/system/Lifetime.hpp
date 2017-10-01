#ifndef XANTHUS_SYSTEM_LIFETIME_HPP
#define XANTHUS_SYSTEM_LIFETIME_HPP

#include "WorldTime.hpp"

#include "entity/World.hpp"

#include "component/LifetimeComponent.hpp"

#include "system/Skeleton.hpp"

namespace xanthus
{

namespace assemblage
{
class Factory;
}

namespace system
{

class Lifetime : public Skeleton<component::LifetimeComponent>
{
public:
    Lifetime(entity::World& world, WorldTime& worldTime, assemblage::Factory& factory);
    ~Lifetime() = default;

    void Update();

private:
    WorldTime& m_worldTime;
    assemblage::Factory& m_factory;
};

}
}

#endif // XANTHUS_SYSTEM_LIFETIME_HPP
