#ifndef XANTHUS_SYSTEM_LIFETIME_HPP
#define XANTHUS_SYSTEM_LIFETIME_HPP

#include "entity/World.hpp"

#include "component/LifetimeComponent.hpp"

#include "system/Skeleton.hpp"

namespace xanthus
{
namespace system
{

class Lifetime : public Skeleton<component::LifetimeComponent>
{
public:
    using TimeUnit = entity::World::TimeUnit;

    Lifetime(entity::World& world);
    ~Lifetime() = default;

    void Update();
};

}
}

#endif // XANTHUS_SYSTEM_LIFETIME_HPP
