# Copyright (C) 2017 by Godlike
# This code is licensed under the MIT license (MIT)
# (http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project(Sandbox)

find_package(Threads)
find_package(SleipnirEngine REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(ASSEMBLAGE_SOURCES
    assemblage/Factory.cpp
    assemblage/Factory.hpp

    assemblage/ProjectileFactory.cpp
    assemblage/ProjectileFactory.hpp
)

set(COMPONENT_SOURCES
    component/ControlComponent.hpp
    component/EntitySnapComponent.hpp
    component/FollowPositionComponent.hpp
    component/GridComponent.hpp
    component/MoveComponent.hpp
)

set(SYSTEM_SOURCES
    system/animation/Filter.hpp
    system/animation/Linear.cpp
    system/animation/Linear.hpp
    system/animation/Move.cpp
    system/animation/Move.hpp
    system/animation/Follow.cpp
    system/animation/Follow.hpp

    system/Gameplay.cpp
    system/Gameplay.hpp

    system/Input.cpp
    system/Input.hpp
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
    controller/Zone.cpp
    controller/Zone.hpp
)

add_executable(${PROJECT_NAME}
    main.cpp

    Application.cpp
    Application.hpp

    Systems.cpp
    Systems.hpp

    WorldTime.hpp

    ${COMPONENT_SOURCES}
    ${SYSTEM_SOURCES}

    ${ASSEMBLAGE_SOURCES}

    ${UTIL_SOURCES}

    ${CONTROLLER_SOURCES}
)

target_link_libraries(
    ${PROJECT_NAME}

    Sleipnir::Engine
)
