# Copyright (C) 2017 by Godlike
# This code is licensed under the MIT license (MIT)
# (http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project(Xanthus)

find_package(UnicornRender REQUIRED)
# find_package(PegasusPhysics REQUIRED)

include_directories(SYSTEM ${Unicorn_INCLUDE_DIRS} ${Pegasus_INCLUDE_DIRS})
link_directories(${Unicorn_LIBRARY_DIRS} ${Pegasus_LIBRARY_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

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
)

set(UTIL_SOURCES
    util/Config.hpp
    util/TemplateCounter.hpp
    util/Types.hpp
)

add_executable(${PROJECT_NAME}
    main.cpp
    Application.cpp

    ${ENTITY_SOURCES}
    ${COMPONENT_SOURCES}
    ${SYSTEM_SOURCES}

    ${UTIL_SOURCES}
)

target_link_libraries(${PROJECT_NAME} ${Unicorn_LIB} ${Pegasus_LIB})