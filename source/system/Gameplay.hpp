#ifndef XANTHUS_SYSTEM_GAMEPLAY_HPP
#define XANTHUS_SYSTEM_GAMEPLAY_HPP

#include "controller/Zone.hpp"

#include <sleipnir/ecs/system/ISystem.hpp>

#include <Arion/SimpleShapeIntersectionDetector.hpp>

#include <tulpar/TulparAudio.hpp>

namespace xanthus
{
namespace system
{

class Gameplay : public sleipnir::ecs::system::ISystem
{
public:
    Gameplay();
    ~Gameplay() = default;

    void Init(tulpar::TulparAudio& audio);
    void Update() override;

private:
    controller::Zone& m_zone;

    arion::intersection::SimpleShapeIntersectionDetector m_detector;
};

}
}

#endif // XANTHUS_SYSTEM_GAMEPLAY_HPP
