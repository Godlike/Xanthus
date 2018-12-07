#include "Systems.hpp"

#include "assemblage/Factory.hpp"

#include "util/ScopeProfiler.hpp"

#include <iostream>

namespace xanthus
{

Systems::Systems(sleipnir::SleipnirEngine& engine, assemblage::Factory& factory)
    : m_engine(engine)
    , m_followAnimation(engine.GetEntityWorld(), engine.GetWorldTime())
    , m_moveAnimation(engine.GetEntityWorld(), engine.GetWorldTime())
    , m_input(engine.GetRender(), engine.GetWorldTime(), engine.GetSystems(), factory)
    , m_gameplay()
{
    //! Register gameplay audio
    m_gameplay.Init(engine.GetAudio());

    //! Registers for window creation events
    m_input.Init();

    sleipnir::ecs::Systems& systems = m_engine.GetSystems();

    {
        using DefaultPriority = sleipnir::ecs::Systems::DefaultPriority;

        systems.Add(&m_input, DefaultPriority::Input + 0x0010);
        systems.Add(&m_followAnimation, DefaultPriority::Physics + 0x0010);
        systems.Add(&m_moveAnimation, DefaultPriority::Physics + 0x0010);
        systems.Add(&m_gameplay, DefaultPriority::Render + 0x0010);
    }
}

Systems::~Systems()
{
    sleipnir::ecs::Systems& systems = m_engine.GetSystems();

    systems.Delete(&m_gameplay);
    systems.Delete(&m_moveAnimation);
    systems.Delete(&m_followAnimation);
    systems.Delete(&m_input);
}

}
