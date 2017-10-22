#ifndef XANTHUS_SYSTEMS_HPP
#define XANTHUS_SYSTEMS_HPP

#include "entity/World.hpp"

#include "system/animation/Follow.hpp"
#include "system/animation/Move.hpp"
#include "system/Input.hpp"
#include "system/Intent.hpp"
#include "system/Lifetime.hpp"
#include "system/physics/Physics.hpp"
#include "system/Render.hpp"
#include "system/SnapToGrid.hpp"
#include "system/SnapToEntity.hpp"
#include "system/Time.hpp"
#include "system/Timer.hpp"

#include <unicorn/Settings.hpp>
#include <unicorn/UnicornRender.hpp>

namespace xanthus
{

namespace assemblage
{
    class Factory;
}

class Systems
{
public:
    Systems(unicorn::Settings& settings
        , WorldTime& worldTime
        , entity::World& world
        , assemblage::Factory& factory
    );

    ~Systems() = default;

    void Update(WorldTime::TimeUnit duration);

    bool IsValid() const;
    unicorn::UnicornRender& GetRender() { return m_unicornRender; }

private:
    friend class assemblage::Factory;

    unicorn::UnicornRender m_unicornRender;

    system::Time m_time;
    system::Lifetime m_lifetime;
    system::Timer m_timer;
    system::Intent m_intents;
    system::SnapToGrid m_snapToGrid;
    system::SnapToEntity m_snapToEntity;
    system::physics::Physics m_physics;
    system::animation::Follow m_followAnimation;
    system::animation::Move m_moveAnimation;
    system::Render m_render;
    system::Input m_input;

    bool m_valid;
};
}

#endif // XANTHUS_SYSTEMS_HPP
