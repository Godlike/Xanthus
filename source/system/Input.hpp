#ifndef XANTHUS_SYSTEM_INPUT_HPP
#define XANTHUS_SYSTEM_INPUT_HPP

#include "entity/Entity.hpp"

#include <unicorn/system/Window.hpp>

#include <unicorn/UnicornRender.hpp>

#include <set>

namespace xanthus
{

class WorldTime;

namespace assemblage
{
class Factory;
}

namespace system
{

class Intent;
class Render;
class Time;

class Input
{
public:
    Input(unicorn::UnicornRender& render
        , WorldTime& worldTime
        , Time& timeSystem
        , Intent& intents
        , Render& renderSystem
        , assemblage::Factory& factory
    );

    ~Input();

    void Init();
    void Update();

private:
    using Window = unicorn::system::Window;

    void CreateProjectile();

    void DropProjectile(entity::Entity entity) const;
    void CompleteProjectile(entity::Entity entity);
    void IterateProjectile(entity::Entity entity);

    void BindWindow(Window* pWindow);
    void UnbindWindow(Window* pWindow);

    void OnWindowCreated(Window* pWindow);
    void OnWindowDestroyed(Window* pWindow);

    void OnWindowMousePosition(Window* pWindow, std::pair<double, double> pos);

    unicorn::UnicornRender& m_unicornRender;

    WorldTime& m_worldTime;
    Time& m_timeSystem;
    Intent& m_intents;
    Render& m_renderSystem;
    assemblage::Factory& m_factory;

    std::vector<Window*> m_windows;

    std::set<unicorn::system::input::Key> m_lastKeys;
};

}
}

#endif // XANTHUS_SYSTEM_INPUT_HPP
