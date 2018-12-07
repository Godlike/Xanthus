#ifndef XANTHUS_SYSTEMS_HPP
#define XANTHUS_SYSTEMS_HPP

#include "system/animation/Follow.hpp"
#include "system/animation/Move.hpp"
#include "system/Gameplay.hpp"
#include "system/Input.hpp"

#include <sleipnir/ecs/entity/World.hpp>

#include <tulpar/TulparAudio.hpp>
#include <tulpar/TulparConfigurator.hpp>

#include <unicorn/utility/Settings.hpp>
#include <unicorn/UnicornRender.hpp>

namespace xanthus
{

namespace assemblage
{
    class Factory;
}

class Systems
{
public:
    Systems(sleipnir::SleipnirEngine& engine, assemblage::Factory& factory);

    ~Systems();

private:
    friend class assemblage::Factory;

    sleipnir::SleipnirEngine& m_engine;

    system::animation::Follow m_followAnimation;
    system::animation::Move m_moveAnimation;
    system::Input m_input;
    system::Gameplay m_gameplay;
};

}

#endif // XANTHUS_SYSTEMS_HPP
