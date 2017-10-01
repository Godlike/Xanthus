# Copyright (C) 2017 by Godlike
# This code is licensed under the MIT license (MIT)
# (http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project(Xanthus)

find_package(Threads)
find_package(UnicornRender REQUIRED)
find_package(PegasusPhysics REQUIRED)

include_directories(SYSTEM ${Unicorn_INCLUDE_DIRS} ${Pegasus_INCLUDE_DIRS})
link_directories(${Unicorn_LIBRARY_DIRS} ${Pegasus_LIBRARY_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(ASSEMBLAGE_SOURCES
    assemblage/Factory.cpp
    assemblage/Factory.hpp
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

    component/LifetimeComponent.hpp
    component/PositionComponent.hpp
    component/PhysicsComponent.hpp
    component/RenderComponent.hpp
)

set(SYSTEM_SOURCES
    system/Skeleton.hpp
    system/Skeleton.imp

    system/Input.cpp
    system/Input.hpp

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

    system/Time.cpp
    system/Time.hpp
)

set(UTIL_SOURCES
    util/Config.hpp
    util/TemplateCounter.hpp
    util/Types.hpp

    util/QSBR.hpp
    util/QSBR.cpp
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
)

target_link_libraries(${PROJECT_NAME} ${Unicorn_LIB} ${Pegasus_LIB} ${CMAKE_THREAD_LIBS_INIT})
