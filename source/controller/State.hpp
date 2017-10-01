#ifndef XANTHUS_CONTROLLER_STATE_HPP
#define XANTHUS_CONTROLLER_STATE_HPP

#include "entity/Entity.hpp"
#include "entity/World.hpp"

#include "component/PositionComponent.hpp"
#include "component/DummyComponent.hpp"

#include "system/Skeleton.hpp"

#include <unicorn/utility/templates/Singleton.hpp>

#include <memory>

namespace xanthus
{
namespace controller
{

class State : public unicorn::utility::templates::Singleton<State>
{
public:
    void Init(entity::World& world);

    void SelectNext();
    entity::Entity const& GetSelected() const { return m_selectedEntity; }

private:
    friend class unicorn::utility::templates::Singleton<State>;

    State();

    State(const State& other) = delete;
    State& operator=(const State& other) = delete;

    ~State() = default;

    struct DummySystem : public system::Skeleton<component::PositionComponent, component::DummyComponent>
    {
        DummySystem(entity::World& world);
        ~DummySystem() = default;

        using Skeleton<component::PositionComponent, component::DummyComponent>::GetEntities;
    };

    std::unique_ptr<DummySystem> m_pDummySystem;
    entity::Entity m_selectedEntity;
};

}
}

#endif // XANTHUS_CONTROLLER_STATE_HPP
