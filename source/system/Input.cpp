#include "system/Input.hpp"

#include "WorldTime.hpp"

#include "assemblage/Factory.hpp"

#include "controller/State.hpp"

#include "component/LifetimeComponent.hpp"
#include "component/PositionComponent.hpp"
#include "component/ValueAnimationComponent.hpp"

#include "system/animation/Linear.hpp"
#include "system/Time.hpp"
#include "system/Render.hpp"

#include <unicorn/video/Graphics.hpp>

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

                case Key::F:
                {
                    CreateProjectile();
                    break;
                }
                case Key::Y:
                {
                    using Dummy = Factory::Orders::Dummy;

                    std::random_device rd;
                    std::mt19937 randEngine(rd());

                    std::uniform_real_distribution<> posDistribution(-30.0f, 30.0f);

                    m_factory.orders.dummies.push(Dummy{
                        glm::vec3{
                            posDistribution(randEngine)
                            , posDistribution(randEngine)
                            , posDistribution(randEngine)
                        }
                    });
                    break;
                }
                case Key::Tab:
                {
                    controller::State::Instance().SelectNext();
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
        using ParticleEffect = Factory::Orders::ParticleEffect;

        for (MouseButton const& mouse : pressedMouse)
        {
            switch (mouse)
            {
                case MouseButton::MouseLeft:
                {
                    m_factory.orders.particleEffects.push(ParticleEffect{
                        glm::vec3{0, 0.0f, 0}
                        , WorldTime::TimeUnit(std::chrono::seconds(5))
                        , 2
                        , ParticleEffect::Type::Down
                    });
                    break;
                }
                case MouseButton::MouseRight:
                {
                    m_factory.orders.particleEffects.push(ParticleEffect{
                        glm::vec3{0, 20.0f, 0}
                        , WorldTime::TimeUnit(std::chrono::seconds(5))
                        , 2
                        , ParticleEffect::Type::Up
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

    m_lastKeys = std::move(pressedKeys);
}

void Input::CreateProjectile()
{
    using namespace component;
    using Projectile = assemblage::Factory::Orders::Projectile;

    WorldTime::TimeUnit now = m_worldTime.GetTime();

    ValueAnimationComponent animComp;
    animComp.startPosition = glm::vec3{0, 0, 0};
    animComp.target = controller::State::Instance().GetSelected();
    animComp.startTime = now;
    animComp.endTime = now + WorldTime::TimeUnit(std::chrono::seconds(1));
    animComp.pFilter = &system::animation::Linear;

    animComp.onFail.connect(this, &Input::DropProjectile);

    animComp.onIteration.connect(this, &Input::IterateProjectile);

    animComp.onComplete.connect(this, &Input::DropProjectile);
    animComp.onComplete.connect(this, &Input::CompleteProjectile);

    m_factory.orders.projectiles.push(Projectile{
        animComp.startPosition
        , animComp
    });
}

void Input::DropProjectile(entity::Entity entity) const
{
    using namespace component;

    LifetimeComponent lifetimeComp = entity.AddComponent<LifetimeComponent>();
    lifetimeComp.deadline = m_worldTime.GetTime();
}

void Input::CompleteProjectile(entity::Entity entity)
{
    using namespace component;
    using ParticleEffect = assemblage::Factory::Orders::ParticleEffect;

    PositionComponent posComp = entity.GetComponent<PositionComponent>();

    m_factory.orders.particleEffects.push(ParticleEffect{
        posComp.position
        , WorldTime::TimeUnit(std::chrono::seconds(1))
        , 16
        , ParticleEffect::Type::Down
    });
}

void Input::IterateProjectile(entity::Entity entity)
{
    using namespace component;
    using ParticleEffect = assemblage::Factory::Orders::ParticleEffect;

    PositionComponent posComp = entity.GetComponent<PositionComponent>();

    {
        static std::random_device rd;
        static std::mt19937 randEngine(rd());

        static std::bernoulli_distribution particleChance(0.1);

        if (particleChance(randEngine))
        {
            m_factory.orders.particleEffects.push(ParticleEffect{
                posComp.position
                , WorldTime::TimeUnit(std::chrono::milliseconds(500))
                , 2
                , ParticleEffect::Type::Down
            });
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
