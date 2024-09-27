#ifndef ENGINE_SOURCES_CONTAINERS_DENSE_MAP_FILE_NODE_H
#define ENGINE_SOURCES_CONTAINERS_DENSE_MAP_FILE_NODE_H

#include <memory>
#include <utility>

namespace egg::Containers::Internal
{
    template <typename Key, typename Type>
    struct DenseMapNode final
    {
        using ValueType = std::pair<Key, Type>;

        template <typename... Args>
        explicit DenseMapNode(const std::size_t Position, Args&&... Arguments): Next { Position },
                                                                                Value { std::forward<Args>(Arguments)... }
        {
        }

        template <typename AllocatorType, typename... Args>
        DenseMapNode(std::allocator_arg_t, const AllocatorType& Allocator, const std::size_t Position, Args&&... Arguments)
            : Next { Position },
              Value { std::make_obj_using_allocator<ValueType>(Allocator, std::forward<Args>(Arguments)...) }
        {
        }

        template <typename AllocatorType>
        DenseMapNode(std::allocator_arg_t, const AllocatorType& Allocator, const DenseMapNode& Other)
            : Next { Other.Next },
              Value { std::make_obj_using_allocator<ValueType>(Allocator, Other.Value) }
        {
        }

        template <typename AllocatorType>
        DenseMapNode(std::allocator_arg_t, const AllocatorType& Allocator, DenseMapNode&& Other)
            : Next { Other.Next },
              Value { std::make_obj_using_allocator<ValueType>(Allocator, std::move(Other.Value)) }
        {
        }

        std::size_t Next;
        ValueType Value;
    };
}

#endif // ENGINE_SOURCES_CONTAINERS_DENSE_MAP_FILE_NODE_H
