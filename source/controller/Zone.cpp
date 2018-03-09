#include "controller/Zone.hpp"

#include "util/Config.hpp"

#include "component/LifetimeComponent.hpp"
#include "component/PositionComponent.hpp"

#include <random>

namespace xanthus
{
namespace controller
{

// Zone::

void Zone::Reset(uint64_t seed, assemblage::Factory& factory)
{
    Deinitialize();

    m_seed = seed;
    m_rngesus.seed(seed);

    Initialize(factory);
}

void Zone::ResetPlayer(assemblage::Factory& factory)
{
    if (nullptr != m_player.get() && m_player->IsValid())
    {
        m_player->AddComponent<component::LifetimeComponent>();
    }

    InitializePlayer(factory);
}

void Zone::RegisterSphere(entity::Entity entity)
{
    m_spheres.push_back(entity);
}

// Zone::Wall::

constexpr glm::vec3 const Zone::Wall::s_normal = glm::vec3(0, 0, 1);
constexpr glm::vec3 const Zone::Wall::s_offset = glm::vec3(
    (0.5f - s_size.first * 0.5f) * Zone::s_boxUnit
    , 0.5f * Zone::s_boxUnit + 0.1f
    , -20.0f
);

Zone::Wall::Wall(Hole const& hole, int32_t border)
{
    std::vector<std::pair<int32_t, int32_t>> empties;

    // initialize wall data
    for (int32_t x = 0; x < Wall::s_size.first; ++x)
    {
        for (int32_t y = 0; y < Wall::s_size.second; ++y)
        {
            glm::vec2 holeVec(
                (hole.coordinates.first - x) * s_boxUnit
                , (hole.coordinates.second - y) * s_boxUnit
            );

            if (glm::length(holeVec) > hole.radius)
            {
                m_data[x][y] = BlockInfo(1, 1);
            }
            else
            {
                empties.emplace_back(x, y);
                m_data[x][y] = BlockInfo(0, 0);
            }
        }
    }

    // expand cells next to the hole
    for (auto const& coords : empties)
    {
        for (int32_t x = -1; x < 2; ++x)
        {
            for (int32_t y = -1; y < 2; ++y)
            {
                TryExpand(x + coords.first, y + coords.second);
            }
        }
    }

    // expand everything

    for (int32_t x = border; x < Wall::s_size.first - border; ++x)
    {
        for (int32_t y = border; y < Wall::s_size.second - border; ++y)
        {
            TryExpand(x, y);
        }
    }
}

Zone::Wall::~Wall()
{
    ResetEntities();
}

void Zone::Wall::BuildEntities(assemblage::Factory& factory)
{
    ResetEntities();

    for (int32_t x = 0; x < Wall::s_size.first; ++x)
    {
        for (int32_t y = 0; y < Wall::s_size.second; ++y)
        {
            BlockInfo const s = m_data[x][y];

            if (s != BlockInfo(0, 0))
            {
                glm::vec3 position = glm::vec3(
                    (static_cast<float>(x) + static_cast<float>(s.first - 1) * 0.5f) * Zone::s_boxUnit
                    , (static_cast<float>(Wall::s_size.second - 1) - (static_cast<float>(y) + static_cast<float>(s.second - 1) * 0.5f)) * Zone::s_boxUnit
                    , 0
                );

                m_entities.push_back(
                    factory.CreateBox(
                        arion::Box(position + Wall::s_offset
                            , glm::vec3{1, 0, 0} * static_cast<float>(s.first) * Zone::s_boxUnit * 0.5f
                            , glm::vec3{0, 1, 0} * static_cast<float>(s.second) * Zone::s_boxUnit * 0.5f
                            , glm::vec3{0, 0, 1} * Wall::s_thickness * 0.5f
                        )
                    )
                );
            }
        }
    }
}

void Zone::Wall::TryExpand(int32_t x, int32_t y)
{
    if (m_data[x][y] == BlockInfo(1, 1))
    {
        std::array<int32_t, 4> exp { 0, 0, 0, 0 }; // bottom left top right
        std::array<bool, 4> check {true, true, true, true};

        while (check[0] || check[1])
        {
            int32_t db = exp[0] + (check[0] ? 1 : 0);
            int32_t dl = exp[1] + (check[1] ? 1 : 0);

            if (check[0] && (y - db) < 0)
            {
                check[0] = false;
                db -= 1;
            }

            if (check[1] && (x + dl) >= Wall::s_size.first)
            {
                check[1] = false;
                dl -= 1;
            }

            // check bottom
            for (int32_t i = x - exp[3]; check[0] && i <= x + dl; ++i)
            {
                check[0] = (m_data[i][y - db] == BlockInfo(1, 1));
            }

            if (check[0])
            {
                exp[0] = db;
            }

            // check left
            for (int32_t j = y - db; check[1] && j <= y + exp[2]; ++j)
            {
                check[1] = (m_data[x + dl][j] == BlockInfo(1, 1));
            }

            if (check[1])
            {
                exp[1] = dl;
            }
        }

        while (check[2] || check[3])
        {
            int32_t dt = exp[2] + (check[2] ? 1 : 0);
            int32_t dr = exp[3] + (check[3] ? 1 : 0);

            if (check[2] && (y + dt) >= Wall::s_size.second)
            {
                check[2] = false;
                dt -= 1;
            }

            if (check[3] && (x - dr) < 0)
            {
                check[3] = false;
                dr -= 1;
            }

            // check top
            for (int32_t i = x - dr; check[2] && i <= x + exp[1]; ++i)
            {
                check[2] = (m_data[i][y + dt] == BlockInfo(1, 1));
            }

            if (check[2])
            {
                exp[2] = dt;
            }

            // check right
            for (int32_t j = y - exp[0]; check[3] && j <= y + dt; ++j)
            {
                check[3] = (m_data[x - dr][j] == BlockInfo(1, 1));
            }

            if (check[3])
            {
                exp[3] = dr;
            }
        }

        int32_t dw = exp[1] + exp[3] + 1;
        int32_t dh = exp[0] + exp[2] + 1;

        int32_t startX = x - exp[3];
        int32_t startY = y - exp[0];

        for (int32_t i = 0; i < dw; ++i)
        {
            for (int32_t j = 0; j < dh; ++j)
            {
                m_data[startX + i][startY + j] = BlockInfo(0, 0);
            }
        }

        m_data[startX][startY] = BlockInfo(dw, dh);
    }
}

void Zone::Wall::ResetEntities()
{
    for (auto & entity : m_entities)
    {
        if (entity.IsValid())
        {
            entity.AddComponent<component::LifetimeComponent>();
        }
    }
}

// Zone::

Zone::Zone()
    : isCompleted(false)
    , m_seed(0)
    , m_sphereRadius(0.0)
    , m_player(nullptr)
    , m_floor(nullptr)
    , m_wall(nullptr)
    , m_rngesus(m_seed)
{

}

void Zone::Deinitialize()
{
    isCompleted = false;

    m_wall.reset(nullptr);

    if (nullptr != m_player.get() && m_player->IsValid())
    {
        m_player->AddComponent<component::LifetimeComponent>();
    }

    m_player.reset(nullptr);

    if (nullptr != m_floor.get() && m_floor->IsValid())
    {
        m_floor->AddComponent<component::LifetimeComponent>();
    }

    m_floor.reset(nullptr);

    for (auto & entity : m_playArea)
    {
        if (entity.IsValid())
        {
            entity.AddComponent<component::LifetimeComponent>();
        }
    }

    for (auto & entity : m_spheres)
    {
        if (entity.IsValid())
        {
            entity.AddComponent<component::LifetimeComponent>();
        }
    }

    m_spheres.clear();
}

void Zone::Initialize(assemblage::Factory& factory)
{
    InitializePlayer(factory);
    InitializeFloor(factory);

    InitializeHole();
    InitializeWall(factory);

    // Debug hole output
    // factory.CreateBox(m_hole.shape
    //     , std::uniform_real_distribution<>(1, 1)
    //     , std::numeric_limits<double>::quiet_NaN()
    //     , false
    // );
}

void Zone::InitializePlayer(assemblage::Factory& factory)
{
    std::uniform_real_distribution<> radiusDistribution(0.2, 0.4);

    static constexpr float playerSize = 0.25f;

    m_sphereRadius = radiusDistribution(m_rngesus);
    m_player.reset(
        new entity::Entity(
            factory.CreateBox(
                arion::Box(
                    glm::vec3{0, 1.0f, 0}
                    , glm::vec3{1, 0, 0} * playerSize
                    , glm::vec3{0, 1, 0} * playerSize
                    , glm::vec3{0, 0, 1} * playerSize
                )
                , std::uniform_real_distribution<>(0, 0)
                , 1.0f
            )
        )
    );
}

void Zone::InitializeFloor(assemblage::Factory& factory)
{
    std::uniform_real_distribution<> radiusDistribution(0.2, 0.4);

    m_floor.reset(
        new entity::Entity(
            factory.CreatePlane(
                arion::Plane(
                    glm::vec3{0, 0, 0}
                    , glm::vec3{0, 1, 0}
                )
            )
        )
    );

    static constexpr float playBorderThickness = 0.25f;

    // left
    m_playArea[0] = entity::Entity(factory.CreateBox(
        arion::Box(
            glm::vec3{
                -Zone::s_playArea.first
                , 0.1f + playBorderThickness
                , 0.0f
            }
            , glm::vec3{1, 0, 0} * playBorderThickness
            , glm::vec3{0, 1, 0} * playBorderThickness
            , glm::vec3{0, 0, 1} * Zone::s_playArea.second
        )
        , std::uniform_real_distribution<>(0, 0)
    ));

    // right
    m_playArea[1] = entity::Entity(factory.CreateBox(
        arion::Box(
            glm::vec3{
                Zone::s_playArea.first
                , 0.1f + playBorderThickness
                , 0.0f
            }
            , glm::vec3{1, 0, 0} * playBorderThickness
            , glm::vec3{0, 1, 0} * playBorderThickness
            , glm::vec3{0, 0, 1} * Zone::s_playArea.second
        )
        , std::uniform_real_distribution<>(0, 0)
    ));

    // forward
    m_playArea[2] = entity::Entity(factory.CreateBox(
        arion::Box(
            glm::vec3{
                0.0f
                , 0.1f + playBorderThickness
                , -Zone::s_playArea.second
            }
            , glm::vec3{1, 0, 0} * Zone::s_playArea.first
            , glm::vec3{0, 1, 0} * playBorderThickness
            , glm::vec3{0, 0, 1} * playBorderThickness
        )
        , std::uniform_real_distribution<>(0, 0)
    ));

    // back
    m_playArea[3] = entity::Entity(factory.CreateBox(
        arion::Box(
            glm::vec3{
                0.0f
                , 0.1f + playBorderThickness
                , Zone::s_playArea.second
            }
            , glm::vec3{1, 0, 0} * Zone::s_playArea.first
            , glm::vec3{0, 1, 0} * playBorderThickness
            , glm::vec3{0, 0, 1} * playBorderThickness
        )
        , std::uniform_real_distribution<>(0, 0)
    ));

    // status
    m_status.reset(
        new entity::Entity(
            factory.CreateBox(
                arion::Box(
                    glm::vec3{
                        0.0f
                        , 0.1f + playBorderThickness * 2.0f
                        , -Zone::s_playArea.second
                    }
                    , glm::vec3{1, 0, 0} * 0.5f
                    , glm::vec3{0, 1, 0} * 0.5f
                    , glm::vec3{0, 0, 1} * 0.5f
                )
                , std::uniform_real_distribution<>(0.5, 0.5)
            )
        )
    );
}

void Zone::InitializeHole()
{
    m_hole = Hole();
    m_hole.radius = std::uniform_real_distribution<>((4 * m_sphereRadius), (6 * m_sphereRadius))(m_rngesus);

    int32_t const wallBorder = WallBorderWidth();

    std::uniform_int_distribution<int32_t> coordsXDistribution(wallBorder, Wall::s_size.first - wallBorder);
    std::uniform_int_distribution<int32_t> coordsYDistribution(wallBorder, Wall::s_size.second - wallBorder);

    m_hole.coordinates = {coordsXDistribution(m_rngesus), coordsYDistribution(m_rngesus)};
    m_hole.position = glm::vec3(
        static_cast<float>(m_hole.coordinates.first) * Zone::s_boxUnit
        , static_cast<float>(Wall::s_size.second - 1 - m_hole.coordinates.second) * Zone::s_boxUnit
        , -Wall::s_thickness
    ) + Wall::s_offset;

    m_hole.shape = arion::Box(
        m_hole.position
        , glm::vec3{1, 0, 0} * m_hole.radius
        , glm::vec3{0, 1, 0} * m_hole.radius
        , glm::vec3{0, 0, 1} * Wall::s_thickness * 0.5f
    );
}

void Zone::InitializeWall(assemblage::Factory& factory)
{
    m_wall.reset(new Wall(m_hole, WallBorderWidth()));
    m_wall->BuildEntities(factory);
}

}
}
