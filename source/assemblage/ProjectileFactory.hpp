#ifndef XANTHUS_ASSEMBLAGE_PROJECTILE_FACTORY_HPP
#define XANTHUS_ASSEMBLAGE_PROJECTILE_FACTORY_HPP

#include <sleipnir/SleipnirEngine.hpp>

#include <sleipnir/ecs/WorldTime.hpp>
#include <sleipnir/ecs/entity/Entity.hpp>
#include <sleipnir/ecs/system/Render.hpp>

#include <glm/glm.hpp>

namespace xanthus
{

namespace system
{
    class Render;
}

namespace assemblage
{

class Factory;

class ProjectileFactory
{
public:
    using WorldTime = sleipnir::ecs::WorldTime;

    struct Order
    {
        glm::vec3 position;
        sleipnir::ecs::entity::Entity target;

        wink::signal< wink::slot<void(sleipnir::ecs::entity::Entity)> > onComplete;
    };

    ProjectileFactory(sleipnir::SleipnirEngine& engine, assemblage::Factory& factory);

    static void Delete(sleipnir::ecs::entity::Entity entity);

    void Create(sleipnir::ecs::entity::Entity entity, Order const& order);
    void Tick(sleipnir::ecs::entity::Entity entity);
    void Hit(sleipnir::ecs::entity::Entity entity);

private:
    WorldTime const& m_worldTime;
    assemblage::Factory& m_factory;
    sleipnir::ecs::system::Render& m_render;
};

}
}

#endif // XANTHUS_ASSEMBLAGE_PROJECTILE_FACTORY_HPP
