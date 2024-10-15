cmake_minimum_required(VERSION 3.28)
project(VulkanEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(VulkanEngine_lib
        Sources/ECS/Systems/Renderer/Renderer.cpp
        Sources/Engine/Engine.cpp
        Sources/Scene/Scene.cpp
        Sources/Events/Types/WindowCreated/WindowCreated.cpp
        Sources/Events/Types/WindowCreated/WindowCreated.cpp
        Sources/Events/Types/WindowEvent/WindowEvent.cpp
        Sources/GUI/Window/Window.cpp
        Sources/Math/Math.cpp
        Sources/Config/Config.h
        Sources/ECS/Containers/PagedVector/PagedVector.h
        Sources/ECS/Containers/SparseSet/SparseSet.h
        Sources/ECS/Containers/Storage/Storage.h
        Sources/ECS/Systems/Renderer/Renderer.h
        Sources/ECS/Systems/System.h
        Sources/ECS/Traits/BasicEntityTraits.h
        Sources/ECS/Traits/ComponentTraits.h
        Sources/ECS/Traits/EntityTraits.h
        Sources/ECS/Traits/PageSizeTraits.h
        Sources/Engine/Engine.h
        Sources/Events/Dispatcher.h
        Sources/Events/Types/WindowCreated/WindowCreated.h
        Sources/GUI/Window/Window.h
        Sources/Math/Math.h
        Sources/Scene/Scene.h
        Sources/Constants/Memory.h
        Sources/Containers/DenseMap/DenseMap.h
        Sources/Containers/DenseMap/Internal/DenseMapNode.h
        Sources/Containers/CompressedPair/CompressedPair.h
        Sources/Containers/CompressedPair/Internal/CompressedPairElement.h
        Sources/ECS/Entity.h
        Sources/Events/Types/WindowEvent/WindowEvent.h
        Sources/ECS/Containers/Container.h
        Sources/Containers/Container.h
        Sources/Containers/IterableAdaptor.h
        Sources/Containers/PointerImitator.h
        Sources/Type/TypeInfo.h
        Sources/Type/TypeID.h
        Sources/Hash/Internal/Traits/Murmur2Traits.h
        Sources/Hash/Internal/Traits/Fnv1aTraits.h
        Sources/ECS/Containers/Registry/Registry.h
        Sources/Events/Delegate.h
        Sources/Events/Traits/FunctionPointerTraits.h
        Sources/Type/Internal/FunctionNameUtils.h
        Sources/Type/Internal/TypeNameUtils.h
        Sources/Type/Internal/TypeIndex.h
        Sources/Type/Traits/Constness.h
        Sources/Algorithms/Sorting/StandardSorting.h
        Sources/Type/Traits/Deduction.h
        Sources/Events/Traits/FunctorOverloadTraits.h
        Sources/Events/Traits/Internal/FunctorOverloadTraits.h
        Sources/Containers/DenseMap/Internal/DenseMapIterator.h
        Sources/Containers/DenseMap/Internal/DenseMapLocalIterator.h
        Sources/ECS/Containers/SparseSet/Internal/SparseSetIterator.h
        Sources/ECS/Containers/Storage/Internal/StorageIterator.h
        Sources/ECS/Containers/PagedVector/Internal/PagedVectorIterator.h
        Sources/Type/Traits/Capabilities.h
        Sources/Events/Signal.h
)

find_package(Vulkan REQUIRED)
target_link_libraries(VulkanEngine_lib PRIVATE Vulkan::Vulkan)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
find_package(glfw3 REQUIRED)
target_link_libraries(VulkanEngine_lib PUBLIC glfw3::glfw3)

include(CompileShaders)
CompileShaders(VulkanEngine_lib "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders")

add_subdirectory(Tests)