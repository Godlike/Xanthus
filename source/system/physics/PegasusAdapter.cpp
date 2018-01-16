#include "system/physics/PegasusAdapter.hpp"

#include <Arion/Shape.hpp>

#include "util/Config.hpp"

namespace xanthus
{
namespace system
{
namespace physics
{

PegasusAdapter::PegasusAdapter()
    : primitiveCount(0)
    , m_scene(pegasus::scene::Scene::GetInstance())
{

}

void PegasusAdapter::Init()
{
    m_scene.Initialize(pegasus::scene::AssetManager::GetInstance());

    // Forces
    {
        double const force = 9.8;

        m_forces.resize(static_cast<std::size_t>(Force::Count));

        m_forces[static_cast<std::size_t>(Force::Down)] = std::make_unique<pegasus::scene::Force<pegasus::force::StaticField>>(pegasus::force::StaticField(glm::dvec3{ 0, -force, 0 }));
        m_forces[static_cast<std::size_t>(Force::Up)] = std::make_unique<pegasus::scene::Force<pegasus::force::StaticField>>(pegasus::force::StaticField(glm::dvec3{ 0, force, 0 }));
    }
}

pegasus::scene::Handle PegasusAdapter::SpawnBody(SpawnInfo const& info)
{
    using pegasus::scene::Primitive;

    assert(nullptr != info.pShape);

    Primitive* pPrimitive = nullptr;

    pegasus::mechanics::Body body;
    body.material.damping = info.damping;

    body.linearMotion.position = info.position;
    body.linearMotion.velocity = info.velocity;

    Primitive::Type primitiveType = Primitive::Type::DYNAMIC;

    if (!std::isnan(info.mass))
    {
        body.material.SetMass(info.mass);
    }
    else
    {
        body.material.SetInfiniteMass();
        primitiveType = Primitive::Type::STATIC;
    }

    {
        using Type = arion::SimpleShape::Type;

        switch (info.pShape->type)
        {
            case Type::PLANE:
            {
                pPrimitive = new pegasus::scene::Plane(primitiveType
                    , body
                    , *static_cast<arion::Plane*>(info.pShape)
                );

                break;
            }
            case Type::BOX:
            {
                pPrimitive = new pegasus::scene::Box(primitiveType
                    , body
                    , *static_cast<arion::Box*>(info.pShape)
                );

                break;
            }
            case Type::SPHERE:
            {
                pPrimitive = new pegasus::scene::Sphere(primitiveType
                    , body
                    , *static_cast<arion::Sphere*>(info.pShape)
                );

                break;
            }
            default:
            {
                return pegasus::scene::Handle();
                break;
            }
        }
    }

    primitives.push_back(pPrimitive);
    ++primitiveCount;

    m_forces[static_cast<std::size_t>(info.force)]->Bind(*pPrimitive);

    return pPrimitive->GetBodyHandle();
}

void PegasusAdapter::DeleteBody(pegasus::scene::Handle bodyHandle)
{
    assert(pegasus::scene::Handle() != bodyHandle);

    auto primitiveIt = std::find_if(
        primitives.begin()
        , primitives.end()
        , [=](auto const& ptr) -> bool
        {
            return ptr->GetBodyHandle() == bodyHandle;
        }
    );

    if (primitiveIt != primitives.end())
    {
        pegasus::scene::Primitive* const pPrimitive = *primitiveIt;

        primitives.erase(primitiveIt);
        --primitiveCount;

        for (auto& forceIt : m_forces)
        {
            forceIt->Unbind(*pPrimitive);
        }

        delete pPrimitive;
    }
}

void PegasusAdapter::Run(WorldTime::TimeUnit tick)
{
    m_scene.ComputeFrame(
        static_cast<float>(tick.count())
        * (static_cast<float>(decltype(tick)::period::num)
            / static_cast<float>(decltype(tick)::period::den)
        )
    );
}

}
}
}
