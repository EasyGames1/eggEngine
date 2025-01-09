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
        Sources/Containers/PagedVector/PagedVector.h
        Sources/ECS/Containers/SparseSet/SparseSet.h
        Sources/ECS/Containers/Storage/Storage.h
        Sources/ECS/Systems/Renderer/Renderer.h
        Sources/ECS/Systems/System.h
        Sources/ECS/Traits/BasicEntityTraits.h
        Sources/ECS/Traits/ComponentTraits.h
        Sources/ECS/Traits/EntityTraits.h
        Sources/ECS/Traits/PageSizeTraits.h
        Sources/Engine/Engine.h
        Sources/Events/Types/WindowCreated/WindowCreated.h
        Sources/GUI/Window/Window.h
        Sources/Math/Math.h
        Sources/Scene/Scene.h
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
        Sources/Types/TypeInfo/TypeInfo.h
        Sources/Hash/Hash.h
        Sources/Events/Delegate.h
        Sources/Events/Traits/FunctionPointerTraits.h
        Sources/Types/TypeInfo/Internal/FunctionNameUtils.h
        Sources/Types/TypeInfo/Internal/TypeNameUtils.h
        Sources/Types/TypeInfo/Internal/TypeIndex.h
        Sources/Events/Traits/FunctorOverloadTraits.h
        Sources/Events/Traits/Internal/FunctorOverloadTraits.h
        Sources/Containers/DenseMap/Internal/DenseMapIterator.h
        Sources/Containers/DenseMap/Internal/DenseMapLocalIterator.h
        Sources/ECS/Containers/SparseSet/Internal/SparseSetIterator.h
        Sources/ECS/Containers/Storage/Internal/StorageIterator.h
        Sources/Containers/PagedVector/Internal/PagedVectorIterator.h
        Sources/Events/Signal.h
        Sources/Events/Action.h
        Sources/Math/Traits/Capabilities.h
        Sources/Events/Traits/Internal/FunctionPointerTraits.h
        Sources/Events/Traits/FunctionTraits.h
        Sources/Events/Sink.h
        Sources/Events/EventLoop.h
        Sources/Events/EventLoopInterface.h
        Sources/Events/Dispatcher.h
        Sources/Memory/Constants.h
        Sources/Memory/Deleter.h
        Sources/Containers/CompressedPair/CompressedBase.h
        Sources/Hash/Traits/FNV1aTraits.h
        Sources/Hash/Traits/Murmur2Traits.h
        Sources/Hash/Literals.h
        Sources/Types/Types.h
        Sources/ECS/Registry.h
        Sources/ECS/Containers/Lifecycle.h
        Sources/Types/Capabilities/Internal/IsInstanceOf.h
        Sources/Types/Capabilities/Internal/IsAnyOf.h
        Sources/ECS/Containers/Group/PoolGroup.h
        Sources/ECS/Ownership.h
        Sources/Types/Capabilities/Internal/IsAllSame.h
        Sources/ECS/Containers/Group/Internal/GroupIterator.h
        Sources/Types/Capabilities/Capabilities.h
        Sources/Types/Constness.h
        Sources/Types/Deduction/Deduction.h
        Sources/Types/Capabilities/Internal/IsUniqueIn.h
        Sources/Types/Capabilities/Internal/IsAllUnique.h
        Sources/Types/Capabilities/Internal/IsContainedIn.h
        Sources/Types/Capabilities/Internal/IsApplicable.h
        Sources/Types/Deduction/Internal/RemoveTupleType.h
        Sources/Types/Deduction/Internal/RemoveTupleIndex.h
        Sources/Types/Deduction/Internal/FilterTuple.h
        Sources/Types/Deduction/Internal/CombineTuples.h
        Sources/Types/Deduction/Internal/MemberObjectPointerTraits.h
        Sources/ECS/Containers/Group/Internal/PoolGroupViewer.h
        Sources/Types/Deduction/Internal/TypeIndexIn.h
        Sources/Events/ConnectionArgument.h
        Sources/Types/Deduction/Internal/InstantiateTuple.h
        Sources/Types/Capabilities/Internal/IsPredicateTrait.h
        Sources/Types/Capabilities/Internal/IsAllOf.h
        Sources/Types/Capabilities/Internal/IsNoneOf.h
        Sources/ECS/Containers/Group/Group.h
        Sources/Types/Deduction/Internal/CommonTypeOf.h
        Sources/Types/Deduction/Internal/TupleSize.h
        Sources/ECS/Containers/Group/Internal/PoolTraits.h
)

target_include_directories(VulkanEngine_lib PRIVATE Sources)

find_package(Vulkan REQUIRED)
target_link_libraries(VulkanEngine_lib PRIVATE Vulkan::Vulkan)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
find_package(glfw3 REQUIRED)
target_link_libraries(VulkanEngine_lib PUBLIC glfw3::glfw3)

include(CompileShaders)
CompileShaders(VulkanEngine_lib "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders")

add_subdirectory(Tests)