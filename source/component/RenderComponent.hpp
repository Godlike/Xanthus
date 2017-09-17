#ifndef XANTHUS_COMPONENT_RENDER_COMPONENT_HPP
#define XANTHUS_COMPONENT_RENDER_COMPONENT_HPP

#include "util/Types.hpp"

#include <unicorn/video/geometry/MeshDescriptor.hpp>

namespace xanthus
{
namespace component
{

struct RenderComponent : public Component
{
    unicorn::video::geometry::MeshDescriptor* pObject;
};

}
}

#endif // XANTHUS_COMPONENT_RENDER_COMPONENT_HPP
