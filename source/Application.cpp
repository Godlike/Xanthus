#include "Application.hpp"

#include "controller/Zone.hpp"

#include <iostream>

namespace xanthus
{

// Application

Application::Application(sleipnir::SleipnirEngine& engine)
    : m_engine(engine)
    , m_systems(engine.GetSystems())
    , m_lastFrameTime(0)
    , m_realTime(true)
    , m_factory(engine)
    , m_customSystems(engine, m_factory)
{
    m_engine.GetRender().LogicFrame.connect(this, &Application::OnLogicFrame);
}

Application::~Application()
{

}

void Application::Run()
{
    controller::Zone::Instance().Reset(0x1337, m_factory);

    m_engine.GetRender().Run();
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

    m_systems.RunOnce(frameDelta);
    m_factory.ExecuteOrders();

    m_lastFrameTime = m_realTime.ElapsedMilliseconds();
}

}
