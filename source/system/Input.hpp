#ifndef XANTHUS_SYSTEM_INPUT_HPP
#define XANTHUS_SYSTEM_INPUT_HPP

#include <system/Time.hpp>

#include <unicorn/system/Window.hpp>

#include <unicorn/UnicornRender.hpp>

namespace xanthus
{
namespace system
{

class Input
{
public:
    Input(unicorn::UnicornRender& render, Time& timeSystem);
    ~Input();

    void Init();
    void Update();

private:
    void BindWindow(unicorn::system::Window* pWindow);
    void UnbindWindow(unicorn::system::Window* pWindow);

    void OnWindowCreated(unicorn::system::Window* pWindow);
    void OnWindowDestroyed(unicorn::system::Window* pWindow);

    unicorn::UnicornRender& m_render;
    Time& m_timeSystem;

    std::vector<unicorn::system::Window*> m_windows;
};

}
}

#endif // XANTHUS_SYSTEM_INPUT_HPP
