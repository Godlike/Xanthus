#ifndef XANTHUS_INTENT_GRID_MOVE_HPP
#define XANTHUS_INTENT_GRID_MOVE_HPP

#include "controller/Grid.hpp"

#include "intent/Intent.hpp"

#include "entity/Entity.hpp"

#include "component/GridComponent.hpp"

namespace xanthus
{
namespace intent
{

class GridMove : public Intent
{
public:
    using Coordinates = component::GridComponent;

    GridMove(controller::Grid const& grid
        , entity::Entity entity
        , Coordinates target);

    virtual ~GridMove();

    virtual bool IsValid() const override;
    virtual bool Execute(WorldTime::TimeUnit now) override;
    virtual void Discard() override;

private:
    void OnSuccess(entity::Entity entity);

    intent::Intent::State m_state;
    entity::Entity m_entity;
    Coordinates m_target;

};

}
}

#endif // XANTHUS_INTENT_GRID_MOVE_HPP
