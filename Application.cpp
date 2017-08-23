#include "Application.hpp"

#include <unicorn/system/CustomValue.hpp>
#include <unicorn/system/input/Action.hpp>
#include <unicorn/system/input/Key.hpp>
#include <unicorn/system/input/Modifier.hpp>

#include <unicorn/video/Color.hpp>
#include <unicorn/video/Graphics.hpp>
#include <unicorn/video/geometry/Primitives.hpp>

#include <functional>
#include <random>

Application::Application(unicorn::Settings& settings, unicorn::UnicornRender* pRender)
    : m_frameDelta(0)
    , m_lastFrameTime(0)
    , m_inputTime(0)
    , m_inputDirections{0, 0, 0, 0}
    , m_pRender(pRender)
    , m_timer(true)
    , m_pVkRenderer(nullptr)
{
    unicorn::video::Graphics* pGraphics = m_pRender->GetGraphics();

    pGraphics->SetWindowCreationHint(unicorn::system::WindowHint::Decorated,
                                     unicorn::system::CustomValue::True);

    pGraphics->SetWindowCreationHint(unicorn::system::WindowHint::Resizable,
                                     unicorn::system::CustomValue::True);

    auto h = pGraphics->GetMonitors().back()->GetActiveVideoMode().height;
    auto w = pGraphics->GetMonitors().back()->GetActiveVideoMode().width;

    settings.SetApplicationHeight(h);
    settings.SetApplicationWidth(w);

    unicorn::system::Window* pWindow = pGraphics->SpawnWindow(settings.GetApplicationWidth(),
        settings.GetApplicationHeight(),
        settings.GetApplicationName(),
        nullptr,
        nullptr);

    m_pVkRenderer = pGraphics->SpawnRenderer(pWindow);

    m_pVkRenderer->SetBackgroundColor(unicorn::video::Color::LightPink());

    // Binds
    {
        using namespace std::placeholders;

        m_pRender->LogicFrame.connect(this, &Application::OnLogicFrame);

        m_pVkRenderer->Destroyed.connect(this, &Application::OnRendererDestroyed);

        pWindow->MouseButton.connect(this, &Application::OnMouseButton);
        pWindow->Keyboard.connect(this, &Application::OnWindowKeyboard);
    }

    SceneReset();
}

Application::~Application()
{
    if (m_pVkRenderer)
    {
        for (auto& pMeshDescriptor : m_worldObjects)
        {
            m_pVkRenderer->DeleteMesh(&pMeshDescriptor->GetMesh());
            delete pMeshDescriptor;
        }

        delete m_pVkRenderer;
    }
}

void Application::Run()
{
    m_pRender->Run();
}

void Application::OnLogicFrame(unicorn::UnicornRender* /*render*/)
{
    using unicorn::video::geometry::MeshDescriptor;

    const uint64_t currentTime = m_timer.ElapsedMilliseconds().count();
    const uint64_t frameDelta = currentTime - m_lastFrameTime;

    if (frameDelta <= 0.0)
    {
        return;
    }

    m_frameDelta = frameDelta;

    const uint64_t inputTick = 250;

    if (currentTime - m_inputTime >= inputTick)
    {
        // Apply controls
        m_inputTime = currentTime;

        if (!m_objects.empty())
        {
            const float distance = 2.0f;

            float delta[2];

            for (uint8_t i = 0; i < 4; ++i)
            {
                if (currentTime - m_inputDirections[i] >= inputTick)
                {
                    delta[i / 2] += ((i % 2) == 0 ? distance : -distance);
                }
            }

            m_objects.back()->Translate({ delta[0], delta[1], 0.0f });
        }
    }

    m_lastFrameTime = currentTime;
}

void Application::OnMouseButton(unicorn::system::Window::MouseButtonEvent const& mouseButtonEvent)
{
    using unicorn::system::input::MouseButton;
    using unicorn::system::input::Action;
    using unicorn::video::geometry::MeshDescriptor;
    using unicorn::video::geometry::Primitives;

    unicorn::system::input::Action const& action = mouseButtonEvent.action;

    if (action == Action::Release || action == Action::Repeat)
    {
        return;
    }

    unicorn::system::input::MouseButton const& button = mouseButtonEvent.button;

    switch (button)
    {
        case MouseButton::MouseLeft:
        {
            SpawnObject();

            break;
        }
        case MouseButton::MouseRight:
        {
            DeleteObject();

            break;
        }
        default:
        {
            break;
        }
    }
}

void Application::OnWindowKeyboard(unicorn::system::Window::KeyboardEvent const& keyboardEvent)
{
    using unicorn::system::input::Key;
    using unicorn::system::input::Modifier;
    using unicorn::system::input::Action;
    using unicorn::system::MouseMode;

    unicorn::system::input::Action const& action = keyboardEvent.action;

    if (Action::Release == action)
    {
        return;
    }

    const uint64_t currentTime = m_timer.ElapsedMilliseconds().count();

    switch (keyboardEvent.key)
    {
        case Key::J:
        {
            SceneReset();
            break;
        }
        case Key::Left:
        {
            m_inputDirections[0] = currentTime;
            break;
        }
        case Key::Right:
        {
            m_inputDirections[1] = currentTime;
            break;
        }
        case Key::Up:
        {
            m_inputDirections[2] = currentTime;
            break;
        }
        case Key::Down:
        {
            m_inputDirections[3] = currentTime;
            break;
        }
        default:
        {
            break;
        }
    }
}

void Application::OnRendererDestroyed(unicorn::video::Renderer* pRenderer)
{
    if (m_pVkRenderer == pRenderer)
    {
        m_pVkRenderer = nullptr;
    }
}

void Application::SpawnObject()
{
    using unicorn::video::geometry::MeshDescriptor;
    using unicorn::video::geometry::Primitives;

    glm::dvec3 position = {0.0, 0.0, 10.0};

    // Render part
    MeshDescriptor* pObj = new MeshDescriptor(Primitives::Quad(*(m_pVkRenderer->SpawnMesh())));
    pObj->Translate(position);
    pObj->SetColor({static_cast<float>(std::rand() % 255) / 255, static_cast<float>(std::rand() % 255) / 255, static_cast<float>(std::rand() % 255) / 255});
    m_objects.push_back(pObj);
}

void Application::DeleteObject()
{
    if (m_objects.size())
    {
        // Get random obj
        const uint32_t index = std::rand() % m_objects.size();

        // Render part
        {
            auto meshIt = m_objects.begin();

            std::advance(meshIt, index);

            // Fetch object's mesh
            auto const& mesh = (*meshIt)->GetMesh();

            // Erase object
            m_objects.erase(meshIt);

            // Release object's mesh
            m_pVkRenderer->DeleteMesh(&mesh);
        }
    }
}

void Application::SceneReset()
{
    // Step 1. Clear
    // Render part
    if (m_pVkRenderer)
    {
        for (auto& pMeshDescriptor : m_objects)
        {
            m_pVkRenderer->DeleteMesh(&pMeshDescriptor->GetMesh());
            delete pMeshDescriptor;
        }

        for (auto& pMeshDescriptor : m_worldObjects)
        {
            m_pVkRenderer->DeleteMesh(&pMeshDescriptor->GetMesh());
            delete pMeshDescriptor;
        }
    }

    m_worldObjects.clear();
    m_objects.clear();

    // Step 2. Setup
    // Render part
    using unicorn::video::geometry::MeshDescriptor;
    using unicorn::video::geometry::Primitives;

    const glm::dvec3 planePosition = {0.0f, 20.0f, 0.0f};

    MeshDescriptor* pPlane = new MeshDescriptor(Primitives::Quad(*(m_pVkRenderer->SpawnMesh())));
    pPlane->Translate(planePosition);
    pPlane->Rotate(glm::radians(90.0f), {1, 0, 0});
    pPlane->Scale({100.00f, 100.0f, 0.0f});
    pPlane->SetColor({0.18f, 0.31f, 0.31f});
    m_worldObjects.push_back(pPlane);
}
