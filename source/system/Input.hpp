#ifndef XANTHUS_SYSTEM_INPUT_HPP
#define XANTHUS_SYSTEM_INPUT_HPP

#include <sleipnir/ecs/entity/Entity.hpp>
#include <sleipnir/ecs/system/ISystem.hpp>
#include <sleipnir/ecs/system/Render.hpp>
#include <sleipnir/ecs/system/TimeBase.hpp>
#include <sleipnir/ecs/Systems.hpp>
#include <sleipnir/ecs/WorldTime.hpp>

#include <unicorn/system/Window.hpp>

#include <unicorn/UnicornRender.hpp>

#include <set>

namespace xanthus
{

namespace assemblage
{
class Factory;
}

namespace system
{

class Input : public sleipnir::ecs::system::ISystem
{
public:
    using WorldTime = sleipnir::ecs::WorldTime;

    Input(unicorn::UnicornRender& render
        , WorldTime& worldTime
        , sleipnir::ecs::Systems& systems
        , assemblage::Factory& factory
    );

    ~Input();

    void Init();
    void Update() override;

private:
    using Window = unicorn::system::Window;

    void CreateProjectile();

    void DropProjectile(sleipnir::ecs::entity::Entity entity) const;
    void CompleteProjectile(sleipnir::ecs::entity::Entity entity);
    void IterateProjectile(sleipnir::ecs::entity::Entity entity);

    void BindWindow(Window* pWindow);
    void UnbindWindow(Window* pWindow);

    void OnWindowCreated(Window* pWindow);
    void OnWindowDestroyed(Window* pWindow);

    void OnWindowMousePosition(Window* pWindow, std::pair<double, double> pos);

    unicorn::UnicornRender& m_unicornRender;

    WorldTime& m_worldTime;
    sleipnir::ecs::system::TimeBase& m_timeSystem;
    sleipnir::ecs::system::Render& m_renderSystem;
    assemblage::Factory& m_factory;

    std::vector<Window*> m_windows;

    std::set<unicorn::system::input::Key> m_lastKeys;
};

}
}

#endif // XANTHUS_SYSTEM_INPUT_HPP
