#ifndef XANTHUS_SYSTEM_RENDER_HPP
#define XANTHUS_SYSTEM_RENDER_HPP

#include "entity/World.hpp"

#include "component/Flags.hpp"
#include "component/PositionComponent.hpp"
#include "component/RenderComponent.hpp"

#include "system/Skeleton.hpp"

#include <unicorn/Settings.hpp>
#include <unicorn/UnicornRender.hpp>

#include <unicorn/video/Renderer.hpp>
#include <unicorn/video/geometry/MeshDescriptor.hpp>

#include <list>

namespace xanthus
{
namespace system
{

class Render : public Skeleton<component::PositionComponent, component::RenderComponent>
{
public:
    Render(entity::World& world);
    ~Render();

    void Init(unicorn::Settings& settings, unicorn::UnicornRender& render);
    void Update();

private:
    void OnRendererDestroyed(unicorn::video::Renderer* pRenderer);

    component::Flags m_query;

    // Render part
    using MeshDescriptor = unicorn::video::geometry::MeshDescriptor;

    unicorn::UnicornRender m_unicornRender;
    unicorn::UnicornRender* m_pRender;

    unicorn::video::Renderer* m_pVkRenderer;

    std::list<MeshDescriptor*> m_worldObjects;
};

}
}

#endif // XANTHUS_SYSTEM_RENDER_HPP
