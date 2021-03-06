# Copyright (C) 2017 by Godlike
# This code is licensed under the MIT license (MIT)
# (http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project(Xanthus)

find_package(Threads)
find_package(EponaMath REQUIRED)
find_package(ArionCollision REQUIRED)
find_package(UnicornRender REQUIRED)
find_package(PegasusPhysics REQUIRED)

include_directories(
    SYSTEM
    ${Epona_INCLUDE_DIRS}
    ${Arion_INCLUDE_DIRS}
    ${Unicorn_INCLUDE_DIRS}
    ${Pegasus_INCLUDE_DIRS}
)

link_directories(
    ${Epona_LIBRARY_DIRS}
    ${Arion_LIBRARY_DIRS}
    ${Unicorn_LIBRARY_DIRS}
    ${Pegasus_LIBRARY_DIRS}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(ASSEMBLAGE_SOURCES
    assemblage/Factory.cpp
    assemblage/Factory.hpp

    assemblage/GridPlateFactory.cpp
    assemblage/GridPlateFactory.hpp

    assemblage/ProjectileFactory.cpp
    assemblage/ProjectileFactory.hpp
)

set(ENTITY_SOURCES
    entity/Entity.cpp
    entity/Entity.hpp
    entity/Entity.imp

    entity/EntityPool.cpp
    entity/EntityPool.hpp

    entity/World.cpp
    entity/World.hpp
)

set(COMPONENT_SOURCES
    component/Flags.hpp
    component/Flags.imp

    component/DummyComponent.hpp
    component/EntitySnapComponent.hpp
    component/LifetimeComponent.hpp
    component/PhysicsComponent.hpp
    component/PositionComponent.hpp
    component/RenderComponent.hpp
    component/TimerComponent.hpp
    component/FollowPositionComponent.hpp
)

set(SYSTEM_SOURCES
    system/Skeleton.hpp
    system/Skeleton.imp

    system/animation/Filter.hpp
    system/animation/Linear.cpp
    system/animation/Linear.hpp
    system/animation/Move.cpp
    system/animation/Move.hpp
    system/animation/Follow.cpp
    system/animation/Follow.hpp

    system/Input.cpp
    system/Input.hpp

    system/Intent.cpp
    system/Intent.hpp

    system/Lifetime.cpp
    system/Lifetime.hpp

    system/physics/BodyHandle.hpp
    system/physics/PegasusAdapter.cpp
    system/physics/PegasusAdapter.hpp
    system/physics/Physics.cpp
    system/physics/Physics.hpp
    system/physics/PhysicsThread.hpp
    system/physics/PhysicsThread.cpp
    system/physics/SpawnInfo.hpp

    system/Render.cpp
    system/Render.hpp

    system/SnapToEntity.cpp
    system/SnapToEntity.hpp

    system/SnapToGrid.cpp
    system/SnapToGrid.hpp

    system/Time.cpp
    system/Time.hpp

    system/Timer.cpp
    system/Timer.hpp
)

set(UTIL_SOURCES
    util/Config.hpp
    util/Math.hpp
    util/ScopeProfiler.hpp
    util/TemplateCounter.hpp
    util/Types.hpp

    util/QSBR.hpp
    util/QSBR.cpp
)

set(CONTROLLER_SOURCES
    controller/Grid.cpp
    controller/Grid.hpp

    controller/Player.cpp
    controller/Player.hpp

    controller/State.cpp
    controller/State.hpp

    controller/Zone.cpp
    controller/Zone.hpp
)

set(INTENT_SOURCES
    intent/Intent.cpp
    intent/Intent.hpp

    intent/GridMove.cpp
    intent/GridMove.hpp
)


add_executable(${PROJECT_NAME}
    main.cpp

    Application.cpp
    Application.hpp

    Systems.cpp
    Systems.hpp

    WorldTime.hpp

    ${ENTITY_SOURCES}
    ${COMPONENT_SOURCES}
    ${SYSTEM_SOURCES}

    ${ASSEMBLAGE_SOURCES}

    ${UTIL_SOURCES}

    ${CONTROLLER_SOURCES}
    ${INTENT_SOURCES}
)

target_link_libraries(
    ${PROJECT_NAME}

    ${Epona_LIB}
    ${Arion_LIB}
    ${Unicorn_LIB}
    ${Pegasus_LIB}
    ${CMAKE_THREAD_LIBS_INIT}
)
