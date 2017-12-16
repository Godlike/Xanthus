#ifndef XANTHUS_CONTROLLER_PLAYER_HPP
#define XANTHUS_CONTROLLER_PLAYER_HPP

#include "controller/Grid.hpp"

#include "intent/Intent.hpp"

#include "entity/Entity.hpp"

#include <memory>

namespace xanthus
{
namespace intent
{
class Intent;
}

namespace controller
{

class Player
{
public:
    Player(entity::Entity entity
        , Grid& grid
    );

    Player(Player const& other) = delete;
    Player& operator=(Player const& other) = delete;

    ~Player();

    entity::Entity GetEntity() const { return m_entity; }

    intent::IntentPtr Move(int16_t x, int16_t y);

private:
    entity::Entity m_entity;
    Grid& m_grid;

    intent::IntentPtr m_pMoveIntent;
};

}
}

#endif // XANTHUS_CONTROLLER_PLAYER_HPP
