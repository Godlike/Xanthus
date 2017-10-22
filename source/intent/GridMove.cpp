#include "intent/GridMove.hpp"

#include "system/animation/Linear.hpp"

#include "component/MoveComponent.hpp"
#include "component/PositionComponent.hpp"

namespace xanthus
{
namespace intent
{

GridMove::GridMove(controller::Grid const& grid
    , entity::Entity entity
    , Coordinates target
)
    : Intent()
    , m_state(Intent::State::Unknown)
    , m_entity(entity)
    , m_target(target)
{
    controller::Grid::Tile const& tile = grid.GetTile(m_entity.GetComponent<component::GridComponent>());

    bool isValid = false;

    for (auto const& pNeighbour : tile.neighbours)
    {
        // tile coordinates loosely match
        if (pNeighbour->coordinates.x == m_target.x
            && pNeighbour->coordinates.y == m_target.y
        )
        {
            m_target = pNeighbour->coordinates; // make sure that we have valid tile coordinates
            isValid = true;
            break;
        }
    }

    if (!isValid)
    {
        m_state = Intent::State::Invalid;
    }
}

GridMove::~GridMove()
{
    Discard();
}

bool GridMove::IsValid() const
{
    switch (m_state)
    {
        case Intent::State::Unknown:
        case Intent::State::Running:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

bool GridMove::Execute(WorldTime::TimeUnit now)
{
    switch (m_state)
    {
        case Intent::State::Unknown:
        {
            if (!m_entity.HasComponent<component::MoveComponent>())
            {
                component::MoveComponent& move = m_entity.AddComponent<component::MoveComponent>();

                move.entity = m_entity;
                move.startPosition = m_entity.GetComponent<component::PositionComponent>().position;
                move.targetPosition = static_cast<glm::vec3>(m_target);
                move.startTime = now;
                move.speed = 1e-5f;
                move.pFilter = &system::animation::Linear;
                move.onComplete.connect(this, &GridMove::OnSuccess);

                m_state = Intent::State::Running;
            }

            return false;
        }
        case Intent::State::Running:
        {
            return false;
        }
        default:
        {
            return true;
        }
    }
}

void GridMove::Discard()
{
    if (Intent::State::Running == m_state
        && m_entity.IsValid()
    )
    {
        m_entity.DeleteComponent<component::MoveComponent>();
    }

    m_state = Intent::State::Invalid;
}

void GridMove::OnSuccess(entity::Entity entity)
{
    if (m_entity == entity)
    {
        m_entity.GetComponent<component::GridComponent>() = m_target;
        m_state = Intent::State::Finished;
    }
}

}
}
