#ifndef XANTHUS_SYSTEM_SKELETON_HPP
#define XANTHUS_SYSTEM_SKELETON_HPP

#include "entity/EntityPool.hpp"
#include "entity/World.hpp"

#include "component/Flags.hpp"

namespace xanthus
{
namespace system
{

template<class... Comps>
    class Skeleton
{
public:
    Skeleton(entity::World& world);
    ~Skeleton();

    bool IsValid() const { return m_valid; }

protected:
    entity::World::Entities GetEntities();

    entity::World& m_world;
    bool m_valid;

private:
    component::Flags m_query;
};

}
}

#include "system/Skeleton.imp"

#endif // XANTHUS_SYSTEM_SKELETON_HPP
