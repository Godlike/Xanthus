#include "system/Input.hpp"

#include <unicorn/video/Graphics.hpp>

namespace xanthus
{
namespace system
{

Input::Input(unicorn::UnicornRender& render, Time& timeSystem)
    : m_render(render)
    , m_timeSystem(timeSystem)
{

}

Input::~Input()
{
    m_render.GetGraphics()->WindowCreated.disconnect(this, &Input::OnWindowCreated);

    for (unicorn::system::Window* pWindow : m_windows)
    {
        UnbindWindow(pWindow);
    }
}

void Input::Init()
{
    m_render.GetGraphics()->WindowCreated.connect(this, &Input::OnWindowCreated);
}

void Input::Update()
{
    using namespace unicorn::system;

    std::set<input::Key> pressedKeys;
    std::set<input::MouseButton> pressedMouse;

    // Poll for pressed keys
    for (Window* pWindow : m_windows)
    {
        auto const& windowKeys = pWindow->GetPressedKeys();
        auto const& windowMouse = pWindow->GetPressedMouseButtons();

        pressedKeys.insert(windowKeys.begin(), windowKeys.end());
        pressedMouse.insert(windowMouse.begin(), windowMouse.end());
    }

    // Apply keys
    for (input::Key const& key : pressedKeys)
    {
        switch (key)
        {
            case input::Key::Num_Add:
            {
                m_timeSystem.factor /= 0.75f;
                break;
            }
            case input::Key::Num_Subtract:
            {
                m_timeSystem.factor *= 0.75f;
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // Apply mouse
    for (input::MouseButton const& mouse : pressedMouse)
    {
        switch (mouse)
        {
            default:
            {
                break;
            }
        }
    }
}

void Input::OnWindowCreated(unicorn::system::Window* pWindow)
{
    BindWindow(pWindow);

    m_windows.push_back(pWindow);
}

void Input::OnWindowDestroyed(unicorn::system::Window* pWindow)
{
    auto it = std::find(m_windows.begin(), m_windows.end(), pWindow);

    if (it != m_windows.end())
    {
        m_windows.erase(it);

        UnbindWindow(pWindow);
    }
}

void Input::BindWindow(unicorn::system::Window* pWindow)
{
    pWindow->Destroyed.connect(this, &Input::OnWindowDestroyed);
}

void Input::UnbindWindow(unicorn::system::Window* pWindow)
{
    pWindow->Destroyed.disconnect(this, &Input::OnWindowDestroyed);
}

}
}
