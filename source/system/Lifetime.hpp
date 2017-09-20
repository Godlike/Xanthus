#ifndef XANTHUS_SYSTEM_LIFETIME_HPP
#define XANTHUS_SYSTEM_LIFETIME_HPP

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
    using TimeUnit = entity::World::TimeUnit;

    Lifetime(entity::World& world, assemblage::Factory& factory);
    ~Lifetime() = default;

    void Update();

private:
    assemblage::Factory& m_factory;
};

}
}

#endif // XANTHUS_SYSTEM_LIFETIME_HPP
