#ifndef XANTHUS_SYSTEM_GAMEPLAY_HPP
#define XANTHUS_SYSTEM_GAMEPLAY_HPP

#include "controller/Zone.hpp"

#include <Arion/SimpleShapeIntersectionDetector.hpp>

namespace xanthus
{
namespace system
{

class Gameplay
{
public:
    Gameplay();
    ~Gameplay() = default;

    void Update();

private:
    controller::Zone& m_zone;
    arion::SimpleShapeIntersectionDetector m_detector;
};

}
}

#endif // XANTHUS_SYSTEM_GAMEPLAY_HPP
