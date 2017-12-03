#include "assemblage/GridPlateFactory.hpp"

#include "component/PositionComponent.hpp"

#include "system/Render.hpp"

#include "util/Math.hpp"

#include <unicorn/video/Primitives.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace xanthus
{
namespace assemblage
{

using namespace xanthus::component;

GridPlateFactory::GridPlateFactory(system::Render& render)
    : m_render(render)
{

}

void GridPlateFactory::Create(entity::Entity entity, Order order)
{
    glm::vec3 const position{0, 0, 0};

    glm::vec3 const normal{0.0f, -1.0f, 0.0f};

    // Position
    {
        component::PositionComponent& positionComponent = entity.AddComponent<component::PositionComponent>();
        positionComponent.position = position;
    }

    // Render
    {
        using unicorn::video::Primitives;

        std::random_device rd;
        std::mt19937 randEngine(rd());

        std::uniform_real_distribution<> colorDistribution(0.0, 1.0);

        system::Render::Material* pMaterial = new system::Render::Material();
        pMaterial->color = util::math::randvec3(colorDistribution, randEngine);

        system::Render::Mesh* pMesh = m_render.SpawnMesh(*pMaterial);
        Primitives::Quad(*pMesh);

        std::vector<unicorn::video::Vertex> vertices = pMesh->GetVertices();
        for (auto& v : vertices)
        {
            v.pos *= util::Config::GridSide;
            v.pos += order.offset;
        }

        pMesh->SetMeshData(vertices, pMesh->GetIndices());

        component::RenderComponent& renderComp = entity.AddComponent<component::RenderComponent>();
        renderComp.pMesh = pMesh;
        renderComp.pMaterial = pMaterial;

        // renderComp.rotateAngle = std::acos(glm::dot(glm::vec3(0, 0, 1), normal));
        // renderComp.rotateAxes = glm::cross(glm::vec3(0, 0, 1), normal);

        renderComp.pMesh->Rotate(normal * static_cast<float>(std::acos(0)));

        pMesh->SetTranslation(position);
        // if (renderComp.rotateAngle != 0.0f)
        // {
        //     pMesh->modelMatrix = glm::rotate(renderComp.pMesh->modelMatrix, renderComp.rotateAngle, renderComp.rotateAxes);
        // }
    }
}

}
}
