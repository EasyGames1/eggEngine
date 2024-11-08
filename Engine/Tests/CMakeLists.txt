cmake_minimum_required(VERSION 3.27)
project(VulkanEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(VulkanEngine_test WindowCreating.cpp
        ECS/Containers/SparseSet.cpp
        ECS/Containers/Storage.cpp
        Containers/DenseMap.cpp
        Single.h
        Events/Delegate.cpp
        CommonFunctions.h
)

find_package(GTest REQUIRED)
target_link_libraries(VulkanEngine_test PRIVATE GTest::gtest_main)

target_include_directories(VulkanEngine_test PRIVATE "${CMAKE_SOURCE_DIR}/Engine/Sources")
target_link_libraries(VulkanEngine_test PRIVATE VulkanEngine_lib)