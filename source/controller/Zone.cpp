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
        static constexpr float const boxUnit = 1.0f;
        static constexpr std::pair<int32_t, int32_t> const wallSize {20, 10};

        static constexpr int32_t const wallBorder = 4;

        std::mt19937_64 engine(seed);

        std::uniform_real_distribution<> radiusDistribution(0.2, 0.4);
        std::uniform_int_distribution<int32_t> coordsXDistribution(wallBorder, wallSize.first - wallBorder);
        std::uniform_int_distribution<int32_t> coordsYDistribution(wallBorder, wallSize.second - wallBorder);

        m_playerRadius = radiusDistribution(engine);
        m_player.reset(new entity::Entity(factory.CreateSphere(m_playerRadius)));

        std::pair<int32_t, int32_t> hole(coordsXDistribution(engine), coordsYDistribution(engine));
        float const holeRadius = std::uniform_real_distribution<>((2 * m_playerRadius + 0.5), (6 * m_playerRadius + 0.5))(engine);

        glm::vec3 holeCenter { boxUnit * hole.first, boxUnit * hole.second, 0.0f };

        {
            static constexpr glm::vec3 const wallPosition {
                (0.5f - wallSize.first * 0.5f) * boxUnit
                , (0.5f - wallSize.second * 0.5f) * boxUnit
                , -20.0f
            };
/*
            m_wall.push_back(factory.CreateBox(
                glm::vec3(-(wallSize.first + wallSize.second) * boxUnit * 0.5f, 0.0f, wallPosition[2])
                , wallSize.second * boxUnit)
            );

            m_wall.push_back(factory.CreateBox(
                glm::vec3((wallSize.first + wallSize.second) * boxUnit * 0.5f, 0.0f, wallPosition[2])
                , wallSize.second * boxUnit)
            );

            m_wall.push_back(factory.CreateBox(
                glm::vec3(0.0f, (wallSize.second + wallSize.first) * boxUnit * 0.5f, wallPosition[2])
                , wallSize.first * boxUnit)
            );

            m_wall.push_back(factory.CreateBox(
                glm::vec3(0.0f, -(wallSize.second + wallSize.first) * boxUnit * 0.5f, wallPosition[2])
                , wallSize.first * boxUnit)
            );
*/
            std::array<std::array<int32_t, wallSize.second>, wallSize.first> memes;
            std::vector<std::pair<int32_t, int32_t>> empties;

            for (int32_t x = 0; x < wallSize.first; ++x)
            {
                for (int32_t y = 0; y < wallSize.second; ++y)
                {
                    glm::vec3 holeVec(hole.first - x, hole.second - y, 0.0f);

                    if (glm::length(holeVec) > holeRadius)
                    {
                        memes[x][y] = 1;
                    }
                    else
                    {
                        empties.emplace_back(x, y);
                        memes[x][y] = 0;
                    }
                }
            }

            auto tryBoxExpand = [=](auto& memes, int32_t x, int32_t y) -> void
            {
                if (memes[x][y] == 1)
                {
                    std::array<int32_t, 4> exp { 0, 0, 0, 0 }; // bottom left top right

                    while (true)
                    {
                        int32_t db = exp[0] + 1;
                        int32_t dl = exp[1] + 1;

                        if ((y - db) < 0
                            || (x + dl) >= wallSize.first)
                        {
                            break;
                        }

                        bool clear = true;

                        // check bottom
                        for (int32_t i = x - exp[3]; i <= x + dl; ++i)
                        {
                            clear = (memes[i][y - db] == 1);

                            if (!clear)
                            {
                                break;
                            }
                        }

                        if (!clear)
                        {
                            break;
                        }

                        // check left
                        for (int32_t j = y - db; j <= y + exp[2]; ++j)
                        {
                            clear = (memes[x + dl][j] == 1);

                            if (!clear)
                            {
                                break;
                            }
                        }

                        if (!clear)
                        {
                            break;
                        }

                        exp[0] = db;
                        exp[1] = dl;
                    }

                    while (true)
                    {
                        int32_t dt = exp[2] + 1;
                        int32_t dr = exp[3] + 1;

                        if ((y + dt) >= wallSize.second
                            || (x - dr) < 0)
                        {
                            break;
                        }

                        bool clear = true;

                        // check top
                        for (int32_t i = x - dr; i <= x + exp[1]; ++i)
                        {
                            clear = (memes[i][y + dt] == 1);

                            if (!clear)
                            {
                                break;
                            }
                        }

                        if (!clear)
                        {
                            break;
                        }

                        // check right
                        for (int32_t j = y - exp[0]; j <= y + dt; ++j)
                        {
                            clear = (memes[x - dr][j] == 1);

                            if (!clear)
                            {
                                break;
                            }
                        }

                        if (!clear)
                        {
                            break;
                        }

                        exp[2] = dt;
                        exp[3] = dr;
                    }

                    int32_t dw = exp[1] + exp[3] + 1;
                    int32_t dh = exp[0] + exp[2] + 1;

                    int32_t r = std::min(dw, dh);

                    int32_t startX = x - exp[3];
                    int32_t startY = y - exp[0];

                    for (int32_t i = 0; i < r; ++i)
                    {
                        for (int32_t j = 0; j < r; ++j)
                        {
                            memes[startX + i][startY + j] = 0;
                        }
                    }

                    memes[startX][startY] = r;
                }
            };

            // expand cells next to the hole
            for (auto const& coords : empties)
            {
                for (int32_t x = -1; x < 2; ++x)
                {
                    for (int32_t y = -1; y < 2; ++y)
                    {
                        tryBoxExpand(memes, x + coords.first, y + coords.second);
                    }
                }
            }

            std::vector<std::pair<glm::vec3, double>> boxes;

            // expand everything else and build boxes

            for (int32_t x = wallBorder; x < wallSize.first - wallBorder; ++x)
            {
                for (int32_t y = wallBorder; y < wallSize.second - wallBorder; ++y)
                {
                    tryBoxExpand(memes, x, y);
                }
            }

            for (int32_t x = 0; x < wallSize.first; ++x)
            {
                for (int32_t y = 0; y < wallSize.second; ++y)
                {
                    int32_t const s = memes[x][y];

                    if (s > 0)
                    {
                        boxes.emplace_back(
                            glm::vec3(
                                (static_cast<float>(x) + static_cast<float>(s - 1) * 0.5f) * boxUnit
                                , (static_cast<float>(wallSize.second - 1) - (static_cast<float>(y) + static_cast<float>(s - 1) * 0.5f)) * boxUnit
                                , 0
                            )
                            , s * boxUnit
                        );
                    }
                }
            }

            for (auto const& box : boxes)
            {
                m_wall.push_back(factory.CreateBox(box.first + wallPosition, box.second));
            }
        }
    }
}

void Zone::ResetPlayer(assemblage::Factory& factory)
{
    if (nullptr != m_player.get() && m_player->IsValid())
    {
        m_player->AddComponent<component::LifetimeComponent>();
    }

    m_player.reset(new entity::Entity(factory.CreateSphere(m_playerRadius)));
}

Zone::Zone()
    : m_seed(0)
    , m_playerRadius(0.0)
    , m_player(nullptr)
{

}

}
}
