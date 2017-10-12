#ifndef XANTHUS_CONTROLLER_ZONE_HPP
#define XANTHUS_CONTROLLER_ZONE_HPP

#include "assemblage/Factory.hpp"

#include "entity/Entity.hpp"

#include "controller/Grid.hpp"

#include <unicorn/utility/templates/Singleton.hpp>

namespace xanthus
{
namespace controller
{

class Zone : public unicorn::utility::templates::Singleton<Zone>
{
public:
    uint64_t GetSeed() const { return m_seed; }
    void Reset(uint64_t seed, assemblage::Factory& factory);

    entity::Entity GetPlayer() const { return m_player; }

private:
    friend class unicorn::utility::templates::Singleton<Zone>;

    Zone();

    Zone(const Zone& other) = delete;
    Zone& operator=(const Zone& other) = delete;

    ~Zone() = default;

    uint64_t m_seed;
    std::unique_ptr<Grid> m_grid;

    entity::Entity m_player;
};

}
}

#endif // XANTHUS_CONTROLLER_ZONE_HPP
