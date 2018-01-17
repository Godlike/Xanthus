#include "Systems.hpp"

#include "assemblage/Factory.hpp"

#include "util/ScopeProfiler.hpp"

#include <iostream>

namespace xanthus
{

Systems::Systems(unicorn::Settings& settings
    , WorldTime& worldTime
    , entity::World& world
    , assemblage::Factory& factory
)
    : m_time(worldTime, m_physics)
    , m_lifetime(world, worldTime, factory)
    , m_timer(world, worldTime)
    , m_physics(world, worldTime)
    , m_followAnimation(world, worldTime)
    , m_moveAnimation(world, worldTime)
    , m_render(world)
    , m_input(m_unicornRender, worldTime, m_time, m_render, factory)
    , m_valid(true)
{
    if (m_unicornRender.Init())
    {
        //! Registers for window creation events
        m_input.Init();

        //! Initialize physics
        m_physics.Init();

        //! Creates windows and renderers
        m_render.Init(settings, m_unicornRender);
    }
    else
    {
        m_valid = false;

        std::cerr << "Failed to initialize UnicornRender!" << std::endl;
    }
}

void Systems::Update(WorldTime::TimeUnit duration)
{
    {
        util::ScopeProfiler profiler("input");
        m_input.Update();
    }

    {
        util::ScopeProfiler profiler("time");
        m_time.Update(duration);
    }

    {
        util::ScopeProfiler profiler("lifetime");
        m_lifetime.Update();
    }

    {
        util::ScopeProfiler profiler("timer");
        m_timer.Update();
    }

    {
        util::ScopeProfiler profiler("physics sync");
        m_physics.Update();
    }

    {
        util::ScopeProfiler profiler("follow animation");
        m_followAnimation.Update();
    }

    {
        util::ScopeProfiler profiler("move animation");
        m_moveAnimation.Update();
    }

    {
        util::ScopeProfiler profiler("render updates");
        m_render.Update();
    }

}

bool Systems::IsValid() const
{
    return m_valid;
}

}
