#ifndef XANTHUS_APPLICATION_HPP
#define XANTHUS_APPLICATION_HPP

#include <unicorn/Settings.hpp>
#include <unicorn/UnicornRender.hpp>

#include <unicorn/system/Timer.hpp>
#include <unicorn/system/Window.hpp>

#include <unicorn/video/CameraFpsController.hpp>
#include <unicorn/video/geometry/MeshDescriptor.hpp>
#include <unicorn/video/Renderer.hpp>

#include <list>

class Application
{
public:
    Application(unicorn::Settings& settings
        , unicorn::UnicornRender* pRender);
    ~Application();

    void Run();

private:
    static const uint32_t MAX_OBJECT_COUNT = 5;

    void OnLogicFrame(unicorn::UnicornRender* pRender);

    void OnMouseButton(unicorn::system::Window::MouseButtonEvent const& mouseButtonEvent);
    void OnMouseScrolled(unicorn::system::Window* pWindow, std::pair<double, double> pos);

    void OnWindowKeyboard(unicorn::system::Window::KeyboardEvent const& keyboardEvent);

    void OnRendererDestroyed(unicorn::video::Renderer* pRenderer);

    void SpawnObject();
    void DeleteObject();

    void SceneReset();

    uint64_t m_frameDelta;
    uint64_t m_lastFrameTime;

    // Input
    uint64_t m_inputTime;
    uint64_t m_inputDirections[4];

    // Render part
    using MeshDescriptor = unicorn::video::geometry::MeshDescriptor;

    unicorn::UnicornRender* m_pRender;
    unicorn::system::Timer m_timer;

    unicorn::video::Renderer* m_pVkRenderer;

    std::list<MeshDescriptor*> m_objects;
    std::list<MeshDescriptor*> m_worldObjects;
};

#endif // XANTHUS_APPLICATION_HPP
