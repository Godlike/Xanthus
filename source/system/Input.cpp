#include "system/Input.hpp"

#include "WorldTime.hpp"

#include "assemblage/Factory.hpp"

#include "controller/Zone.hpp"

#include "component/PositionComponent.hpp"

#include "system/Time.hpp"
#include "system/Render.hpp"

#include <unicorn/video/Graphics.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <iostream>
#include <random>

namespace xanthus
{
namespace system
{

Input::Input(unicorn::UnicornRender& render
    , WorldTime& worldTime
    , Time& timeSystem
    , Render& renderSystem
    , assemblage::Factory& factory
)
    : m_unicornRender(render)
    , m_worldTime(worldTime)
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

    std::vector<input::Key> newKeys;

    std::set_difference(
        pressedKeys.begin(), pressedKeys.end()
        , m_lastKeys.begin(), m_lastKeys.end()
        , std::back_inserter(newKeys)
    );

    // Apply keys
    {
        using input::Key;

        static float const cameraMovementSpeed = 0.1f;
        static float const timeFactorMultuplier = 0.95f;

        assert(nullptr != m_renderSystem.pCameraController);

        glm::vec3 cameraTranslation{0, 0, 0};

        glm::vec3 const cameraDirection = m_renderSystem.pCameraController->GetDirection();
        glm::vec3 const cameraRight = m_renderSystem.pCameraController->GetRight();
        glm::vec3 const cameraUp = m_renderSystem.pCameraController->GetUp();

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

                case Key::W:
                {
                    cameraTranslation += cameraDirection * cameraMovementSpeed;
                    break;
                }
                case Key::S:
                {
                    cameraTranslation -= cameraDirection * cameraMovementSpeed;
                    break;
                }
                case Key::A:
                {
                    cameraTranslation += cameraRight * cameraMovementSpeed;
                    break;
                }
                case Key::D:
                {
                    cameraTranslation -= cameraRight * cameraMovementSpeed;
                    break;
                }
                case Key::Q:
                {
                    cameraTranslation -= cameraUp * cameraMovementSpeed;
                    break;
                }
                case Key::E:
                {
                    cameraTranslation += cameraUp * cameraMovementSpeed;
                    break;
                }

                default:
                {
                    break;
                }
            }
        }

        m_renderSystem.pCameraController->TranslateWorld(cameraTranslation);
        m_renderSystem.pCameraController->Update();

        controller::Zone& zone = controller::Zone::Instance();

        {
            entity::Entity player = zone.GetPlayer();

            if (!player.HasComponent<component::PhysicsComponent>())
            {
                component::PositionComponent& comp = player.GetComponent<component::PositionComponent>();

                comp.position = m_renderSystem.pCameraController->GetTranslation()
                    + m_renderSystem.pCameraController->GetDirection() * 2.0f
                ;
            }
        }

        for (Key const& key : newKeys)
        {
            switch (key)
            {
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

                case Key::Space:
                {
                    entity::Entity player = zone.GetPlayer();

                    if (!player.HasComponent<component::PhysicsComponent>())
                    {
                        component::PositionComponent& comp = player.GetComponent<component::PositionComponent>();

                        glm::vec3 direction = m_renderSystem.pCameraController->GetDirection();

                        if (0 != glm::length2(cameraTranslation))
                        {
                            direction += glm::normalize(cameraTranslation) / 2.0f;
                        }

                        m_factory.ApplySpherePhysics(player
                            , zone.GetPlayerRadius()
                            , Factory::Orders::ParticleEffect{
                                comp.position
                                , glm::normalize(direction) * 20.0f
                                , {}
                                , {}
                                , Factory::Orders::ParticleEffect::Type::Down
                            }
                        );
                    }
                    break;
                }
                case Key::LeftBracket:
                {
                    zone.Reset(zone.GetSeed() - 1, m_factory);

                    break;
                }
                case Key::RightBracket:
                {
                    zone.Reset(zone.GetSeed() + 1, m_factory);

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
    // {
    //     using input::MouseButton;
    //     using ParticleEffect = Factory::Orders::ParticleEffect;

    //     for (MouseButton const& mouse : pressedMouse)
    //     {
    //         switch (mouse)
    //         {
    //             case MouseButton::MouseLeft:
    //             {
    //                 break;
    //             }
    //             case MouseButton::MouseRight:
    //             {
    //                 break;
    //             }
    //             default:
    //             {
    //                 break;
    //             }
    //         }
    //     }
    // }

    m_lastKeys = std::move(pressedKeys);
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
