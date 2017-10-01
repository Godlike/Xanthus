#include "Systems.hpp"

#include "assemblage/Factory.hpp"

#include <iostream>

namespace xanthus
{

Systems::Systems(unicorn::Settings& settings
    , WorldTime& worldTime
    , entity::World& world
    , assemblage::Factory& factory
)
    : m_time(worldTime)
    , m_lifetime(world, worldTime, factory)
    , m_physics(world, worldTime)
    , m_render(world)
    , m_input(m_unicornRender, m_time, m_render, factory)
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
    m_input.Update();

    m_time.Update(duration);

    m_lifetime.Update();
    m_physics.Update();
    m_render.Update();
}

bool Systems::IsValid() const
{
    return m_valid;
}

}
