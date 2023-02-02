cmake_minimum_required(VERSION 3.12)

project(handover-demo-y1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Add the custom cmake modules
set(CMAKE_CONFIG_PATH ${CMAKE_MODULE_PATH}  "${CMAKE_CURRENT_LIST_DIR}/cmake-modules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CONFIG_PATH}")


# find dependencies
find_package(ZMQ) # Optional


# BehaviorTree.CPP links Boost::coroutine, but does not look for it
find_package(Boost COMPONENTS coroutine REQUIRED)
find_package(BehaviorTreeV3 REQUIRED)

find_package(YARP COMPONENTS os idl_tools dev REQUIRED)

add_subdirectory(bts)
add_subdirectory(interfaces)
add_subdirectory(components)
