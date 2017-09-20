#ifndef XANTHUS_SYSTEMS_HPP
#define XANTHUS_SYSTEMS_HPP

#include "entity/World.hpp"

#include "system/Input.hpp"
#include "system/Lifetime.hpp"
#include "system/Physics.hpp"
#include "system/Render.hpp"
#include "system/Time.hpp"

#include <unicorn/Settings.hpp>
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
    using TimeUnit = entity::World::TimeUnit;

    Systems(unicorn::Settings& settings
        , entity::World& world
        , assemblage::Factory& factory
    );

    ~Systems() = default;

    void Update(TimeUnit duration);

    bool IsValid() const;
    unicorn::UnicornRender& GetRender() { return m_unicornRender; }

private:
    friend class assemblage::Factory;

    unicorn::UnicornRender m_unicornRender;

    system::Time m_time;
    system::Lifetime m_lifetime;
    system::Physics m_physics;
    system::Render m_render;
    system::Input m_input;

    bool m_valid;
};
}

#endif // XANTHUS_SYSTEMS_HPP
