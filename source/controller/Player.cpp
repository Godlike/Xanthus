#include "controller/Player.hpp"

#include "intent/GridMove.hpp"

#include "component/GridComponent.hpp"
#include "component/LifetimeComponent.hpp"

namespace xanthus
{
namespace controller
{

Player::Player(entity::Entity entity
    , Grid& grid
)
    : m_entity(entity)
    , m_grid(grid)
{

}

Player::~Player()
{
	if (m_entity.IsValid())
    {
        m_entity.AddComponent<component::LifetimeComponent>();
    }
}

intent::IntentPtr Player::Move(int16_t x, int16_t y)
{
    component::GridComponent& gridComponent = m_entity.GetComponent<component::GridComponent>();

    m_pMoveIntent.reset(new intent::GridMove(m_grid
        , m_entity
        , component::GridComponent(gridComponent.x + x, gridComponent.y + y)
    ));

    return m_pMoveIntent;
}

}
}
