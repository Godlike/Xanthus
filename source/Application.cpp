#include "Application.hpp"

#include "controller/Zone.hpp"

#include <iostream>

namespace xanthus
{

// Application

Application::Application(unicorn::Settings& settings)
    : m_worldTime()
    , m_lastFrameTime(0)
    , m_realTime(true)
    , m_world()
    , m_systems(settings, m_worldTime, m_world, m_factory)
    , m_factory(m_worldTime, m_world, m_systems)
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
    controller::Zone::Instance().Reset(0x1337, m_factory);

    m_systems.GetRender().Run();
}

void Application::OnLogicFrame(unicorn::UnicornRender* /*render*/)
{
    const TimeUnit currentTime = m_realTime.ElapsedMilliseconds();
    const TimeUnit frameDelta = currentTime - m_lastFrameTime;

    std::cerr << std::endl;

    std::cerr << "[Profiler] #render "
        << std::chrono::duration_cast<std::chrono::microseconds>(frameDelta).count() << "us"
        << std::endl;

    if (frameDelta.count() == 0)
    {
        return;
    }

    m_systems.Update(frameDelta);
    m_factory.ExecuteOrders();

    m_lastFrameTime = m_realTime.ElapsedMilliseconds();
}

}
