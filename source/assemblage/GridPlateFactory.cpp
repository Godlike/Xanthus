#include "assemblage/GridPlateFactory.hpp"

#include "component/PositionComponent.hpp"

#include "system/Render.hpp"

namespace xanthus
{
namespace assemblage
{

using namespace xanthus::component;

GridPlateFactory::GridPlateFactory(system::Render& render)
    : m_render(render)
{

}

void GridPlateFactory::Create(entity::Entity entity, Order const& order)
{
    // Position
    {
        component::PositionComponent& positionComponent = entity.AddComponent<component::PositionComponent>();
        positionComponent.position = order.position;
    }
}

}
}
