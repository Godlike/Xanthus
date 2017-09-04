#ifndef XANTHUS_SYSTEM_RENDER_HPP
#define XANTHUS_SYSTEM_RENDER_HPP

#include "component/RenderComponent.hpp"

#include <set>

namespace xanthus
{
namespace system
{

class Render
{
public:
    Render();
    ~Render();

    void Update();
};

}
}

#endif // XANTHUS_SYSTEM_RENDER_HPP
