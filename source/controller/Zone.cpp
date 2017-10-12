#include "controller/Zone.hpp"

#include "util/Config.hpp"

#include <iostream>
#include <random>

namespace xanthus
{
namespace controller
{

void Zone::Reset(uint64_t seed, assemblage::Factory& factory)
{
    m_seed = seed;
    m_player = entity::Entity();

    m_grid.reset(new Grid());

    std::mt19937_64 engine(seed);

    std::uniform_real_distribution<> plateChance(0.3, 1);
    std::bernoulli_distribution plateGenerator(plateChance(engine));
    std::uniform_int_distribution<> altSwingGenerator(0, 10);

    std::uniform_real_distribution<> altChance(0, 1);
    int16_t const altSwing = altSwingGenerator(engine);

    std::binomial_distribution<> altGenerator(altSwing * 2, altChance(engine));

    std::bernoulli_distribution dummyGenerator(0.2);

    for (uint32_t x = 0; x < 8; ++x)
    {
        for (uint32_t y = 0; y < 8; ++y)
        {
            if (plateGenerator(engine))
            {
                Grid::Coordinates coords(x, y, altGenerator(engine) - altSwing);

                std::cerr << "Creating plate " << x << ":" << y << std::endl;

                m_grid->Add(coords, factory.CreateGridPlate(assemblage::GridPlateFactory::Order{
                    glm::vec3{0, 0, 0}
                }));

                if (m_player.IsValid())
                {
                    if (dummyGenerator(engine))
                    {
                        m_grid->Add(coords, factory.CreateDummy());
                    }
                }
                else
                {
                    m_player = factory.CreateDummy();
                    m_grid->Add(coords, m_player);

                    m_grid->Add(coords, factory.CreateGridPlate(assemblage::GridPlateFactory::Order{
                        glm::vec3{0.0f, 0.0f, -1.0f}
                    }));
                }
            }
        }
    }
}

Zone::Zone()
    : m_seed(0)
    , m_grid(nullptr)
{

}

}
}
