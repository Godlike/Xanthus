#ifndef XANTHUS_APPLICATION_HPP
#define XANTHUS_APPLICATION_HPP

#include "Systems.hpp"

#include "assemblage/Factory.hpp"

#include <sleipnir/ecs/entity/World.hpp>
#include <sleipnir/ecs/Systems.hpp>
#include <sleipnir/ecs/WorldTime.hpp>
#include <sleipnir/SleipnirEngine.hpp>

#include <chrono>
#include <list>

namespace xanthus
{

class Application
{
public:
    using TimeUnit = sleipnir::ecs::WorldTime::TimeUnit;

    Application(sleipnir::SleipnirEngine& engine);
    ~Application();

    void Run();

private:
    static const uint32_t MAX_OBJECT_COUNT = 5;

    void OnLogicFrame(unicorn::UnicornRender* pRender);

    sleipnir::SleipnirEngine& m_engine;
    sleipnir::ecs::Systems& m_systems;

    TimeUnit m_lastFrameTime;
    unicorn::system::Timer m_realTime;

    assemblage::Factory m_factory;

    Systems m_customSystems;
};

}

#endif // XANTHUS_APPLICATION_HPP
