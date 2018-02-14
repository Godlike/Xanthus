#include "system/Gameplay.hpp"

#include "entity/Entity.hpp"

#include "component/PositionComponent.hpp"
#include "component/RenderComponent.hpp"

#include "system/Render.hpp"

#include <unicorn/video/Color.hpp>

namespace xanthus
{
namespace system
{

Gameplay::Gameplay()
    : m_zone(controller::Zone::Instance())
{

}

void Gameplay::Update()
{
    if (!m_zone.isCompleted)
    {
        component::RenderComponent& statusRenderComp = m_zone.GetStatus().GetComponent<component::RenderComponent>();

        controller::Zone::Hole const& hole = m_zone.GetHole();

        arion::Sphere mockup(glm::vec3(0, 0, 0), m_zone.GetSphereRadius());

        std::vector<entity::Entity> const& spheres = m_zone.GetSpheres();

        glm::vec3 statusColor = unicorn::video::Color::Red();

        if (!spheres.empty())
        {
            for (entity::Entity sphere : spheres)
            {
                if (sphere.IsValid())
                {
                    mockup.centerOfMass = sphere.GetComponent<component::PositionComponent>().position;

                    if (std::isnan(mockup.centerOfMass[0])
                        || std::isnan(mockup.centerOfMass[1])
                        || std::isnan(mockup.centerOfMass[2])
                        )
                    {
                        continue;
                    }

                    if (m_detector.CalculateIntersection(&hole.shape, &mockup))
                    {
                        statusColor = unicorn::video::Color::Green();
                        m_zone.isCompleted = true;

                        break;
                    }
                }
            }
        }
        else
        {
            statusColor = glm::vec3{0.5f, 0.5f, 0.5f};
        }

        if (statusColor != statusRenderComp.pMesh->GetMaterial()->GetColor())
        {
            system::Render::Material* pMaterial = new system::Render::Material();
            pMaterial->SetColor(statusColor);

            statusRenderComp.pMesh->SetMaterial(std::shared_ptr<system::Render::Material>(pMaterial));
        }
    }
}

}
}
