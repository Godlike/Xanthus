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

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif ()

add_executable(${PROJECT_NAME} main.cpp Application.cpp)

target_link_libraries(${PROJECT_NAME} ${Unicorn_LIB} ${Pegasus_LIB})
