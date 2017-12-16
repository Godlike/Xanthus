#include "controller/Zone.hpp"

#include "util/Config.hpp"

#include "component/EntitySnapComponent.hpp"
#include "component/LifetimeComponent.hpp"

#include <random>

namespace xanthus
{
namespace controller
{

void Zone::Reset(uint64_t seed, assemblage::Factory& factory)
{
    m_seed = seed;

    Grid* pGrid = new Grid();

    m_grid.reset(pGrid);
    m_player.reset(new Player(factory.CreateDummy(), *pGrid));

    if (m_gridPlate.IsValid())
    {
        m_gridPlate.AddComponent<component::LifetimeComponent>();
    }

    m_gridPlate = factory.CreateGridPlate(assemblage::GridPlateFactory::Order{
        glm::vec3{0.0f, 0.0f, -1.0f}
    });

    std::mt19937_64 engine(seed);

    std::uniform_real_distribution<> plateChance(0.3, 1);
    std::bernoulli_distribution plateGenerator(plateChance(engine));
    std::uniform_int_distribution<> altSwingGenerator(0, 10);

    std::uniform_real_distribution<> altChance(0, 1);
    int16_t const altSwing = altSwingGenerator(engine);

    std::binomial_distribution<> altGenerator(altSwing * 2, altChance(engine));

    std::bernoulli_distribution dummyGenerator(1e-1);

    bool playerSet = false;

    for (uint32_t x = 0; x < 8; ++x)
    {
        for (uint32_t y = 0; y < 8; ++y)
        {
            if (plateGenerator(engine))
            {
                Grid::Coordinates coords(x, y, altGenerator(engine) - altSwing);
                m_grid->CreateTile(coords);

                if (playerSet)
                {
                    if (dummyGenerator(engine))
                    {
                        m_grid->Add(coords, factory.CreateDummy());
                    }
                }
                else
                {
                    playerSet = true;
                    m_grid->Add(coords, m_player->GetEntity());

                    component::EntitySnapComponent& snapComp = m_gridPlate.AddComponent<component::EntitySnapComponent>();

                    snapComp.entity = m_player->GetEntity();
                    snapComp.offset = glm::vec3(0);
                }
            }
        }
    }

    m_grid->BuildConnections();
}

Zone::Zone()
    : m_seed(0)
    , m_grid(new Grid())
    , m_player(new Player(entity::Entity(), *m_grid.get()))
    , m_gridPlate()
{

}

}
}
