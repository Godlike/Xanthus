#ifndef XANTHUS_UTIL_MATH_HPP
#define XANTHUS_UTIL_MATH_HPP

#include <glm/glm.hpp>

namespace xanthus
{
namespace util
{
namespace math
{

template<class Distribution, class Engine>
    inline glm::vec3 randvec3(Distribution& distribution, Engine& engine)
    {
        return glm::vec3{distribution(engine), distribution(engine), distribution(engine)};
    }

}
}
}

#endif // XANTHUS_UTIL_MATH_HPP
