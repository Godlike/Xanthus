#include "system/Render.hpp"

#include <unicorn/video/Color.hpp>
#include <unicorn/video/Graphics.hpp>

#include <unicorn/system/CustomValue.hpp>
#include <unicorn/system/Monitor.hpp>
#include <unicorn/system/Window.hpp>

namespace xanthus
{
namespace system
{

Render::Render(entity::World& world)
    : Skeleton<component::PositionComponent, component::RenderComponent>(world)
{

}

Render::~Render()
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

    m_unicornRender.Deinit();
}

void Render::Init(unicorn::Settings& settings, unicorn::UnicornRender& render)
{
    unicorn::video::Graphics* pGraphics = render.GetGraphics();

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

    m_pVkRenderer->Destroyed.connect(this, &Render::OnRendererDestroyed);
}

void Render::Update()
{
    entity::World::Entities entities = GetEntities();


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
