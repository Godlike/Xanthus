#include "controller/State.hpp"

#include "component/FollowPositionComponent.hpp"
#include "component/LifetimeComponent.hpp"

#include <algorithm>

namespace xanthus
{
namespace controller
{

// State

void State::Init(entity::World& world)
{
    m_pDummySystem.reset(new DummySystem(world));
    SelectNext();
}

void State::SelectNext()
{
    entity::World::Entities entities = m_pDummySystem->GetEntities();

    if (!entities.empty())
    {
        if (m_selectedEntity.IsValid())
        {
            auto entityIterator = std::find_if(entities.begin(), entities.end(), [=](auto const& entity) -> bool
            {
                return entity == m_selectedEntity;
            });

            if (entityIterator != entities.end() && ++entityIterator != entities.end())
            {
                m_selectedEntity = *entityIterator;
            }
            else
            {
                m_selectedEntity = entities.front();
            }
        }
        else
        {
            m_selectedEntity = entities.front();
        }
    }
    else
    {
        m_selectedEntity = entity::Entity();
    }
}

void State::ResolveProjectile(entity::Entity projectile)
{
    component::FollowPositionComponent follow = projectile.GetComponent<component::FollowPositionComponent>();
    follow.target.AddComponent<component::LifetimeComponent>();
}

State::State()
    : m_pDummySystem(nullptr)
    , m_selectedEntity()
{

}

// State::DummySystem

State::DummySystem::DummySystem(entity::World& world)
    : system::Skeleton<component::PositionComponent, component::DummyComponent>(world)
{

}

}
}
