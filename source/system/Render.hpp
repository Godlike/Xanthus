#ifndef XANTHUS_SYSTEM_RENDER_HPP
#define XANTHUS_SYSTEM_RENDER_HPP

#include "entity/World.hpp"

#include "component/Flags.hpp"
#include "component/PositionComponent.hpp"
#include "component/RenderComponent.hpp"

#include "system/Skeleton.hpp"

#include <unicorn/Settings.hpp>
#include <unicorn/UnicornRender.hpp>

#include <unicorn/video/Camera.hpp>
#include <unicorn/video/CameraFpsController.hpp>
#include <unicorn/video/Material.hpp>
#include <unicorn/video/Mesh.hpp>
#include <unicorn/video/Renderer.hpp>
#include <unicorn/video/PerspectiveCamera.hpp>

#include <list>

namespace xanthus
{
namespace system
{

class Render : public Skeleton<component::PositionComponent, component::RenderComponent>
{
public:
    // Render part
    using Mesh = unicorn::video::Mesh;
    using Material = unicorn::video::Material;

    Render(entity::World& world);
    ~Render();

    void Init(unicorn::Settings& settings, unicorn::UnicornRender& render);
    void Update();

    Mesh* SpawnMesh();
    void AddMesh(Mesh* pMesh); //! @todo    get rid of this once Unicorn#120 is resolved
    void DeleteMesh(Mesh* pMesh);

    unicorn::video::CameraFpsController* pCameraController;

private:
    void OnRendererDestroyed(unicorn::video::Renderer* pRenderer);

    unicorn::video::Renderer* m_pVkRenderer;
    unicorn::video::Camera m_camera;
    unicorn::video::PerspectiveCamera* pCameraProjection;
};

}
}

#endif // XANTHUS_SYSTEM_RENDER_HPP
