#ifndef XANTHUS_SYSTEMS_HPP
#define XANTHUS_SYSTEMS_HPP

#include "entity/World.hpp"

#include "system/animation/ValueAnimation.hpp"
#include "system/Input.hpp"
#include "system/Lifetime.hpp"
#include "system/physics/Physics.hpp"
#include "system/Render.hpp"
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
    system::physics::Physics m_physics;
    system::animation::ValueAnimation m_valueAnimation;
    system::Render m_render;
    system::Input m_input;

    bool m_valid;
};
}

#endif // XANTHUS_SYSTEMS_HPP
