#include "system/Render.hpp"

#include <unicorn/video/Color.hpp>
#include <unicorn/video/Graphics.hpp>
#include <unicorn/video/Primitives.hpp>

#include <unicorn/system/CustomValue.hpp>
#include <unicorn/system/Monitor.hpp>
#include <unicorn/system/Window.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace xanthus
{
namespace system
{

Render::Render(entity::World& world)
    : Skeleton<component::PositionComponent, component::RenderComponent>(world)
    , pCameraController(nullptr)
    , m_pVkRenderer(nullptr)
{

}

Render::~Render()
{
    if (m_pVkRenderer)
    {
        delete m_pVkRenderer;
    }
}

void Render::Init(unicorn::Settings& settings, unicorn::UnicornRender& render)
{
    unicorn::video::Graphics* pGraphics = render.GetGraphics();

    pGraphics->SetWindowCreationHint(unicorn::system::WindowHint::Decorated,
        unicorn::system::CustomValue::True);

    pGraphics->SetWindowCreationHint(unicorn::system::WindowHint::Resizable,
        unicorn::system::CustomValue::True);

    auto h = pGraphics->GetMonitors().back()->GetActiveVideoMode().height / 2;
    auto w = pGraphics->GetMonitors().back()->GetActiveVideoMode().width / 2;

    w = 1600;
    h = 900;

    settings.SetApplicationHeight(h);
    settings.SetApplicationWidth(w);

    unicorn::system::Window* pWindow = pGraphics->SpawnWindow(settings.GetApplicationWidth(),
        settings.GetApplicationHeight(),
        settings.GetApplicationName(),
        nullptr,
        nullptr);

    m_pVkRenderer = pGraphics->SpawnRenderer(pWindow, m_camera);

    assert(nullptr != m_pVkRenderer);

    m_pVkRenderer->SetBackgroundColor(unicorn::video::Color::Black());

    m_pVkRenderer->Destroyed.connect(this, &Render::OnRendererDestroyed);

    pCameraProjection = new unicorn::video::PerspectiveCamera(*pWindow, m_camera.projection);
    pCameraController = new unicorn::video::CameraFpsController(m_camera.view);

    //! @todo   remove the following call when Unicorn#121 is resolved
    const_cast<glm::mat4&>(pCameraController->GetModelMatrix()) = glm::mat4(1.0f);
    pCameraController->TranslateLocal(glm::vec3{0.0f, 1.0f, 0.0f});
    pCameraController->SetOrientation(glm::vec3{0.0f, 0.0f, -1.0f});
    pCameraController->UpdateTransformMatrix();
}

void Render::Update()
{
    entity::World::Entities entities = GetEntities();

    for (entity::Entity const& entity : entities)
    {
        component::PositionComponent const& posComp = entity.GetComponent<component::PositionComponent>();
        component::RenderComponent const& renderComp = entity.GetComponent<component::RenderComponent>();

        //! @todo   remove the following call when Unicorn#121 is resolved
        const_cast<glm::mat4&>(renderComp.pMesh->GetModelMatrix()) = glm::mat4(1.0f);
        renderComp.pMesh->SetTranslation(posComp.position);
        renderComp.pMesh->UpdateTransformMatrix();
    }

    std::cerr << "[Render] entity count: " << entities.size() << std::endl;
}

Render::Mesh* Render::SpawnMesh()
{
    Render::Mesh* pMesh = new Render::Mesh();

    //! @todo   uncomment the following call when Unicorn#120 is resolved
    // m_pVkRenderer->AddMesh(pMesh);

    return pMesh;
}

void Render::AddMesh(Mesh* pMesh)
{
    m_pVkRenderer->AddMesh(pMesh);
}

void Render::DeleteMesh(Mesh* pMesh)
{
    m_pVkRenderer->DeleteMesh(pMesh);

    delete pMesh;
}

void Render::OnRendererDestroyed(unicorn::video::Renderer* pRenderer)
{
    if (m_pVkRenderer == pRenderer)
    {
        m_pVkRenderer = nullptr;
    }
}

}
}
