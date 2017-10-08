#ifndef XANTHUS_ASSEMBLAGE_PROJECTILE_FACTORY_HPP
#define XANTHUS_ASSEMBLAGE_PROJECTILE_FACTORY_HPP

#include "WorldTime.hpp"

#include "entity/Entity.hpp"

#include <glm/glm.hpp>

namespace xanthus
{
namespace assemblage
{

class Factory;

class ProjectileFactory
{
public:
    struct Order
    {
        glm::vec3 position;
        entity::Entity target;
    };

    ProjectileFactory(WorldTime const& worldTime, assemblage::Factory& factory);

    static void Delete(entity::Entity entity);

    void Create(entity::Entity entity, Order const& order);
    void Tick(entity::Entity entity);
    void Hit(entity::Entity entity);

private:
    WorldTime const& m_worldTime;
    assemblage::Factory& m_factory;
};

}
}

#endif // XANTHUS_ASSEMBLAGE_PROJECTILE_FACTORY_HPP
