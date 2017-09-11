# Copyright (C) 2017 by Godlike
# This code is licensed under the MIT license (MIT)
# (http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project(XanthusTests)

include_directories(${XANTHUS_ROOT} ${CATCH_ROOT}/include)

set(ENTITY_SOURCES
    ${XANTHUS_ROOT}/entity/Entity.cpp
    ${XANTHUS_ROOT}/entity/Entity.hpp
    ${XANTHUS_ROOT}/entity/Entity.imp

    ${XANTHUS_ROOT}/entity/EntityPool.cpp
    ${XANTHUS_ROOT}/entity/EntityPool.hpp

    ${XANTHUS_ROOT}/entity/World.cpp
    ${XANTHUS_ROOT}/entity/World.hpp
)

set(COMPONENT_SOURCES
    ${XANTHUS_ROOT}/component/Flags.hpp
    ${XANTHUS_ROOT}/component/Flags.imp
)

set(UTIL_SOURCES
    ${XANTHUS_ROOT}/util/Config.hpp
    ${XANTHUS_ROOT}/util/TemplateCounter.hpp
    ${XANTHUS_ROOT}/util/Types.hpp
)

function(xanthus_add_test)
    set(options )
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCE ARGS)
    cmake_parse_arguments(
        xanthus_add_test
        "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
    )

    set(EXEC_NAME "${xanthus_add_test_NAME}Test")

    add_executable(
        ${EXEC_NAME}
        ${xanthus_add_test_SOURCE}
    )

    add_test(
        NAME ${xanthus_add_test_NAME}
        COMMAND ${EXEC_NAME} ${xanthus_add_test_ARGS}
    )
endfunction()

xanthus_add_test(NAME TemplateCounter
    SOURCE TemplateCounterTest.cpp
        ${UTIL_SOURCES}
)

xanthus_add_test(NAME ComponentFlags
    SOURCE ComponentFlagsTest.cpp
        ${COMPONENT_SOURCES}
        ${UTIL_SOURCES}
)

xanthus_add_test(NAME EntityPool
    SOURCE EntityPoolTest.cpp
        ${ENTITY_SOURCES}
        ${COMPONENT_SOURCES}
        ${UTIL_SOURCES}
)

xanthus_add_test(NAME Entity
    SOURCE EntityTest.cpp
        ${ENTITY_SOURCES}
        ${UTIL_SOURCES}
)
