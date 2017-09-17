#include "Application.hpp"

#include <iostream>

namespace xanthus
{

// Application

Application::Application(unicorn::Settings& settings)
    : m_lastFrameTime(0)
    , m_realTime(true)
    , m_world()
    , m_systems(settings, m_world)
{
    if (IsValid())
    {
        m_systems.GetRender().LogicFrame.connect(this, &Application::OnLogicFrame);
    }
    else
    {
        std::cerr << "Failed to initialize all systems!" << std::endl;
    }
}

Application::~Application()
{

}

bool Application::IsValid() const
{
    return m_systems.IsValid();
}

void Application::Run()
{
    m_systems.GetRender().Run();
}

// Application::Systems

Application::Systems::Systems(unicorn::Settings& settings, entity::World& world)
    : m_time(world)
    , m_lifetime(world)
    , m_render(world)
    , m_input(m_unicornRender, m_time)
    , m_valid(true)
{
    if (m_unicornRender.Init())
    {
        //! Registers for window creation events
        m_input.Init();

        //! Creates windows and renderers
        m_render.Init(settings, m_unicornRender);
    }
    else
    {
        m_valid = false;

        std::cerr << "Failed to initialize UnicornRender!" << std::endl;
    }
}

void Application::Systems::Update(TimeUnit duration)
{
    m_input.Update();
    m_time.Update(duration);
    m_lifetime.Update();
    m_render.Update();
}

bool Application::Systems::IsValid() const
{
    return m_valid;
}

// Application

void Application::OnLogicFrame(unicorn::UnicornRender* /*render*/)
{
    const TimeUnit currentTime = m_realTime.ElapsedMilliseconds();
    const TimeUnit frameDelta = currentTime - m_lastFrameTime;

    if (frameDelta.count() <= 0)
    {
        return;
    }

    m_systems.Update(frameDelta);

    m_lastFrameTime = currentTime;
}

}
