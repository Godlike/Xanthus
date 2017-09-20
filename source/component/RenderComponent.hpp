#ifndef XANTHUS_COMPONENT_RENDER_COMPONENT_HPP
#define XANTHUS_COMPONENT_RENDER_COMPONENT_HPP

#include "util/Types.hpp"

#include <unicorn/video/Mesh.hpp>
#include <unicorn/video/Material.hpp>

namespace xanthus
{
namespace component
{

struct RenderComponent : public Component
{
    unicorn::video::Mesh* pMesh;
    unicorn::video::Material* pMaterial;

    float rotateAngle = 0.0f;
    glm::vec3 rotateAxes;
};

}
}

#endif // XANTHUS_COMPONENT_RENDER_COMPONENT_HPP
