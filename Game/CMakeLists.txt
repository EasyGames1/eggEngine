cmake_minimum_required(VERSION 3.28)
project(VulkanGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(VulkanGame
        Sources/main.cpp
)

target_include_directories(VulkanGame PRIVATE "${CMAKE_SOURCE_DIR}/Engine/Sources")
target_link_libraries(VulkanGame PRIVATE VulkanEngine_lib)