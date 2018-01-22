#ifndef XANTHUS_SYSTEM_PHYSICS_PEGASUS_ADAPTER_HPP
#define XANTHUS_SYSTEM_PHYSICS_PEGASUS_ADAPTER_HPP

#include "WorldTime.hpp"

#include "system/physics/SpawnInfo.hpp"

#include <pegasus/Scene.hpp>

#include <cstddef>
#include <list>
#include <map>
#include <vector>

namespace xanthus
{
namespace system
{
namespace physics
{

class PegasusAdapter
{
public:
    using Primitives = std::list<pegasus::scene::Primitive*>;

    PegasusAdapter();
    ~PegasusAdapter() = default;

    void Init();

    pegasus::scene::Handle SpawnBody(SpawnInfo const& info);
    void PushBody(pegasus::scene::Handle bodyHandle, glm::vec3 force);
    void DeleteBody(pegasus::scene::Handle bodyHandle);

    void Run(WorldTime::TimeUnit tick);

    Primitives primitives;
    std::size_t primitiveCount;

private:
    using Forces = std::vector<std::unique_ptr<pegasus::scene::Force<pegasus::force::StaticField>>>;

    pegasus::scene::Scene& m_scene;
    Forces m_forces;
};

}
}
}

#endif // XANTHUS_SYSTEM_PHYSICS_PEGASUS_ADAPTER_HPP
