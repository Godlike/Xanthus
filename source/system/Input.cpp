#include "system/Input.hpp"

#include "assemblage/Factory.hpp"

#include "system/Time.hpp"
#include "system/Render.hpp"

#include <unicorn/video/Graphics.hpp>

#include <iostream>

namespace xanthus
{
namespace system
{

Input::Input(unicorn::UnicornRender& render
    , Time& timeSystem
    , Render& renderSystem
    , assemblage::Factory& factory
)
    : m_unicornRender(render)
    , m_timeSystem(timeSystem)
    , m_renderSystem(renderSystem)
    , m_factory(factory)
{

}

Input::~Input()
{
    m_unicornRender.GetGraphics()->WindowCreated.disconnect(this, &Input::OnWindowCreated);

    for (Window* pWindow : m_windows)
    {
        UnbindWindow(pWindow);
    }
}

void Input::Init()
{
    m_unicornRender.GetGraphics()->WindowCreated.connect(this, &Input::OnWindowCreated);
}

void Input::Update()
{
    using namespace unicorn::system;
    using namespace assemblage;

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
    {
        using input::Key;

        static float const cameraMovementSpeed = 0.01f;
        static float const timeFactorMultuplier = 0.95f;

        assert(nullptr != m_renderSystem.pCameraController);

        for (Key const& key : pressedKeys)
        {
            switch (key)
            {
                case Key::Num_Add:
                {
                    m_timeSystem.factor /= timeFactorMultuplier;
                    std::cerr << "Time\tfactor " << m_timeSystem.factor << std::endl;
                    break;
                }
                case Key::Num_Subtract:
                {
                    m_timeSystem.factor *= timeFactorMultuplier;
                    std::cerr << "Time\tfactor " << m_timeSystem.factor << std::endl;
                    break;
                }
                case Key::C:
                {
                    m_windows[0]->SetMouseMode(MouseMode::Captured);
                    break;
                }
                case Key::Escape:
                {
                    m_windows[0]->SetMouseMode(MouseMode::Normal);
                    break;
                }

                case Key::W:
                {
                    m_renderSystem.pCameraController->MoveForward(cameraMovementSpeed);
                    break;
                }
                case Key::S:
                {
                    m_renderSystem.pCameraController->MoveBackward(cameraMovementSpeed);
                    break;
                }
                case Key::A:
                {
                    m_renderSystem.pCameraController->MoveLeft(cameraMovementSpeed);
                    break;
                }
                case Key::D:
                {
                    m_renderSystem.pCameraController->MoveRight(cameraMovementSpeed);
                    break;
                }
                case Key::Q:
                {
                    m_renderSystem.pCameraController->MoveUp(cameraMovementSpeed);
                    break;
                }
                case Key::E:
                {
                    m_renderSystem.pCameraController->MoveDown(cameraMovementSpeed);
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }

    // Apply mouse
    {
        using input::MouseButton;

        for (MouseButton const& mouse : pressedMouse)
        {
            switch (mouse)
            {
                case MouseButton::MouseLeft:
                {
                    m_factory.orders.particleEffects.push(Factory::Orders::ParticleEffect{
                        glm::vec3{0, 0.0f, 0}
                        , entity::World::TimeUnit(5000)
                        , 2
                    });
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}

void Input::BindWindow(Window* pWindow)
{
    pWindow->Destroyed.connect(this, &Input::OnWindowDestroyed);
    pWindow->MousePosition.connect(this, &Input::OnWindowMousePosition);
}

void Input::UnbindWindow(Window* pWindow)
{
    pWindow->Destroyed.disconnect(this, &Input::OnWindowDestroyed);
    pWindow->MousePosition.disconnect(this, &Input::OnWindowMousePosition);
}

void Input::OnWindowCreated(Window* pWindow)
{
    BindWindow(pWindow);

    m_windows.push_back(pWindow);
}

void Input::OnWindowDestroyed(Window* pWindow)
{
    auto it = std::find(m_windows.begin(), m_windows.end(), pWindow);

    if (it != m_windows.end())
    {
        m_windows.erase(it);
    }
}

void Input::OnWindowMousePosition(Window* pWindow, std::pair<double, double> pos)
{
    assert(nullptr != m_renderSystem.pCameraController);

    m_renderSystem.pCameraController->UpdateView(pos.first, pos.second);
}

}
}
