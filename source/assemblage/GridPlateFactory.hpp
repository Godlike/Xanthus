#ifndef XANTHUS_ASSEMBLAGE_GRID_PLATE_FACTORY_HPP
#define XANTHUS_ASSEMBLAGE_GRID_PLATE_FACTORY_HPP

#include "WorldTime.hpp"

#include "entity/Entity.hpp"

#include "component/GridComponent.hpp"

#include <glm/glm.hpp>

namespace xanthus
{

namespace system
{
    class Render;
}

namespace assemblage
{

class Factory;

class GridPlateFactory
{
public:
    struct Order
    {
        glm::vec3 offset;
    };

    GridPlateFactory(system::Render& render);

    void Create(entity::Entity entity, Order order);

private:
    system::Render& m_render;
};

}
}

#endif // XANTHUS_ASSEMBLAGE_GRID_PLATE_FACTORY_HPP
