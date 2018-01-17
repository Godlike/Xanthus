#include "controller/Zone.hpp"

#include "util/Config.hpp"

#include "component/LifetimeComponent.hpp"
#include "component/PositionComponent.hpp"

#include <random>

namespace xanthus
{
namespace controller
{

void Zone::Reset(uint64_t seed, assemblage::Factory& factory)
{
    for (auto & entity : m_wall)
    {
        if (entity.IsValid())
        {
            entity.AddComponent<component::LifetimeComponent>();
        }
    }

    m_wall.clear();

    if (nullptr != m_player.get() && m_player->IsValid())
    {
        m_player->AddComponent<component::LifetimeComponent>();
    }

    m_seed = seed;

    {
        std::pair<float, float> const boxUnit {1.0f, 1.0f};
        std::pair<uint32_t, uint32_t> const wallSize {64, 32};

        glm::vec3 const wallPosition {
            -static_cast<float>(wallSize.first / 2) * boxUnit.first
            , -static_cast<float>(wallSize.second / 2) * boxUnit.second
            , -20.0f
        };

        static uint32_t const wallBorder = 4;

        std::mt19937_64 engine(seed);

        std::uniform_real_distribution<> radiusDistribution(0.3, 0.7);
        std::uniform_int_distribution<uint32_t> coordsXDistribution(wallBorder, wallSize.first - wallBorder);
        std::uniform_int_distribution<uint32_t> coordsYDistribution(wallBorder, wallSize.second - wallBorder);

        m_playerRadius = radiusDistribution(engine);
        m_player.reset(new entity::Entity(factory.CreateSphere(m_playerRadius)));

        std::pair<uint32_t, uint32_t> hole(coordsXDistribution(engine), coordsYDistribution(engine));
        float const holeRadius = std::uniform_real_distribution<>(m_playerRadius + 0.5, 3 * (m_playerRadius + 0.5))(engine);

        glm::vec3 holeCenter { boxUnit.first * hole.first, boxUnit.second * hole.second, 0.0f };
        holeCenter -= wallPosition;

        m_wall.push_back(factory.CreateBox(wallPosition, holeRadius * 10));
    }
}

Zone::Zone()
    : m_seed(0)
    , m_playerRadius(0.0)
    , m_player(nullptr)
{

}

}
}
