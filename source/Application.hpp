#ifndef XANTHUS_APPLICATION_HPP
#define XANTHUS_APPLICATION_HPP

#include "entity/World.hpp"

#include "system/Input.hpp"
#include "system/Lifetime.hpp"
#include "system/Render.hpp"
#include "system/Time.hpp"

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
    Application(unicorn::Settings& settings);
    ~Application();

    bool IsValid() const;
    void Run();

private:
    using TimeUnit = std::chrono::milliseconds;

    static const uint32_t MAX_OBJECT_COUNT = 5;

    class Systems
    {
    public:
        Systems(unicorn::Settings& settings, entity::World& world);
        ~Systems() = default;

        void Update(TimeUnit duration);

        bool IsValid() const;
        unicorn::UnicornRender& GetRender() { return m_unicornRender; }

    private:
        unicorn::UnicornRender m_unicornRender;

        system::Time m_time;
        system::Lifetime m_lifetime;
        system::Render m_render;
        system::Input m_input;

        bool m_valid;
    };

    void OnLogicFrame(unicorn::UnicornRender* pRender);

    TimeUnit m_lastFrameTime;

    unicorn::system::Timer m_realTime;

    entity::World m_world;
    Systems m_systems;
};

}

#endif // XANTHUS_APPLICATION_HPP
