#include "system/Gameplay.hpp"

#include <sleipnir/ecs/component/PositionComponent.hpp>
#include <sleipnir/ecs/component/RenderComponent.hpp>
#include <sleipnir/ecs/entity/Entity.hpp>

#include <unicorn/video/Color.hpp>

namespace xanthus
{
namespace system
{

Gameplay::Gameplay()
    : m_zone(controller::Zone::Instance())
{

}

void Gameplay::Init(tulpar::TulparAudio& audio)
{
    m_zone.InitializeAudio(audio);
}

void Gameplay::Update()
{
    if (!m_zone.isCompleted)
    {
        sleipnir::ecs::component::RenderComponent& statusRenderComp = m_zone.GetStatus().GetComponent<sleipnir::ecs::component::RenderComponent>();

        controller::Zone::Hole const& hole = m_zone.GetHole();

        arion::Sphere mockup(glm::vec3(0, 0, 0), glm::quat(), m_zone.GetSphereRadius());

        std::vector<sleipnir::ecs::entity::Entity> const& spheres = m_zone.GetSpheres();

        glm::vec3 statusColor = unicorn::video::Color::Red();

        if (!spheres.empty())
        {
            for (sleipnir::ecs::entity::Entity sphere : spheres)
            {
                if (sphere.IsValid())
                {
                    mockup.centerOfMass = sphere.GetComponent<sleipnir::ecs::component::PositionComponent>().position;

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
                        const_cast<controller::Zone::Hole&>(hole).sounds.onSuccess.second.Play();
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
            sleipnir::ecs::system::Render::Material* pMaterial = new sleipnir::ecs::system::Render::Material();
            pMaterial->SetColor(statusColor);

            statusRenderComp.pMesh->SetMaterial(std::shared_ptr<sleipnir::ecs::system::Render::Material>(pMaterial));
        }
    }
}

}
}
