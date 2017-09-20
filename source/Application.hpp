#ifndef XANTHUS_APPLICATION_HPP
#define XANTHUS_APPLICATION_HPP

#include "Systems.hpp"

#include "entity/World.hpp"

#include "assemblage/Factory.hpp"

#include <unicorn/Settings.hpp>
#include <unicorn/UnicornRender.hpp>

#include <unicorn/system/Timer.hpp>

#include <chrono>
#include <list>

namespace xanthus
{

class Application
{
public:
    using TimeUnit = entity::World::TimeUnit;

    Application(unicorn::Settings& settings);
    ~Application();

    bool IsValid() const;
    void Run();

private:
    static const uint32_t MAX_OBJECT_COUNT = 5;

    void OnLogicFrame(unicorn::UnicornRender* pRender);

    TimeUnit m_lastFrameTime;

    unicorn::system::Timer m_realTime;

    entity::World m_world;
    Systems m_systems;
    assemblage::Factory m_factory;
};

}

#endif // XANTHUS_APPLICATION_HPP
