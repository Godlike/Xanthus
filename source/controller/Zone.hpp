#ifndef XANTHUS_CONTROLLER_ZONE_HPP
#define XANTHUS_CONTROLLER_ZONE_HPP

#include "assemblage/Factory.hpp"

#include <unicorn/utility/templates/Singleton.hpp>

#include <Arion/Shape.hpp>

namespace xanthus
{
namespace controller
{

class Zone : public unicorn::utility::templates::Singleton<Zone>
{
public:
    struct Hole
    {
        std::pair<int32_t, int32_t> coordinates;
        float radius;
        glm::vec3 position;

        arion::Box shape;
    };

    uint64_t GetSeed() const { return m_seed; }
    void Reset(uint64_t seed, assemblage::Factory& factory);

    void ResetPlayer(assemblage::Factory& factory);
    entity::Entity GetPlayer() const { return *m_player.get(); }
    entity::Entity GetStatus() const { return *m_status.get(); }

    Hole const& GetHole() const { return m_hole; }

    double GetSphereRadius() const { return m_sphereRadius; }
    void RegisterSphere(entity::Entity entity);
    std::vector<entity::Entity> const& GetSpheres() const { return m_spheres; }

    bool isCompleted;

private:
    friend class unicorn::utility::templates::Singleton<Zone>;

    static constexpr float const s_boxUnit = 0.8f;
    static constexpr std::pair<float, float> const s_playArea = std::pair<float, float>(8.0f, 4.5f);

    class Wall
    {
    public:
        static constexpr float const s_thickness = 1.0f;
        static constexpr std::pair<int32_t, int32_t> const s_size = std::pair<int32_t, int32_t>(32, 18);
        static glm::vec3 const s_normal;
        static glm::vec3 const s_offset;

        using BlockInfo = std::pair<int32_t, int32_t>;
        using Column = std::array<BlockInfo, Wall::s_size.second>;
        using Data = std::array<Column, Wall::s_size.first>;

        Wall(Hole const& hole, int32_t border);
        ~Wall();

        void BuildEntities(assemblage::Factory& factory);
        Data const& GetData() const { return m_data; }

        void TryExpand(int32_t x, int32_t y);

    private:
        void ResetEntities();

        std::vector<entity::Entity> m_entities;
        Data m_data;
    };

    Zone();

    Zone(const Zone& other) = delete;
    Zone& operator=(const Zone& other) = delete;

    ~Zone() = default;

    void Deinitialize();
    void Initialize(assemblage::Factory& factory);

    void InitializePlayer(assemblage::Factory& factory);
    void InitializeFloor(assemblage::Factory& factory);

    void InitializeHole();
    void InitializeWall(assemblage::Factory& factory);

    int32_t WallBorderWidth() const { return static_cast<int32_t>(m_hole.radius / s_boxUnit) + 2; }

    uint64_t m_seed;
    double m_sphereRadius;

    std::unique_ptr<entity::Entity> m_player;
    std::unique_ptr<entity::Entity> m_floor;
    std::unique_ptr<entity::Entity> m_status;
    std::array<entity::Entity, 4> m_playArea;

    std::unique_ptr<Wall> m_wall;
    Hole m_hole;

    std::vector<entity::Entity> m_spheres;

    std::mt19937_64 m_rngesus;
};

}
}

#endif // XANTHUS_CONTROLLER_ZONE_HPP
