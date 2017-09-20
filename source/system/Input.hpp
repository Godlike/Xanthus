#ifndef XANTHUS_SYSTEM_INPUT_HPP
#define XANTHUS_SYSTEM_INPUT_HPP

#include <unicorn/system/Window.hpp>

#include <unicorn/UnicornRender.hpp>

namespace xanthus
{

namespace assemblage
{
class Factory;
}

namespace system
{

class Time;
class Render;

class Input
{
public:
    Input(unicorn::UnicornRender& render
        , Time& timeSystem
        , Render& renderSystem
        , assemblage::Factory& factory
    );

    ~Input();

    void Init();
    void Update();

private:
    using Window = unicorn::system::Window;

    void BindWindow(Window* pWindow);
    void UnbindWindow(Window* pWindow);

    void OnWindowCreated(Window* pWindow);
    void OnWindowDestroyed(Window* pWindow);

    void OnWindowMousePosition(Window* pWindow, std::pair<double, double> pos);

    unicorn::UnicornRender& m_unicornRender;
    Time& m_timeSystem;
    Render& m_renderSystem;
    assemblage::Factory& m_factory;

    std::vector<Window*> m_windows;
};

}
}

#endif // XANTHUS_SYSTEM_INPUT_HPP
