#ifndef ENGINE_SOURCES_MEMORY_FILE_DELETER_H
#define ENGINE_SOURCES_MEMORY_FILE_DELETER_H

#include "Containers/CompressedPair/CompressedBase.h"

namespace egg::Memory
{
    template <typename AllocatorParameter>
    class AllocationDeleter : Containers::CompressedBase<AllocatorParameter>
    {
        using BaseType = Containers::CompressedBase<AllocatorParameter>;
        using DeleterAllocatorTraits = std::allocator_traits<AllocatorParameter>;

    public:
        using allocator_type = AllocatorParameter;
        using pointer = typename DeleterAllocatorTraits::pointer;


        constexpr AllocationDeleter() noexcept(std::is_nothrow_default_constructible_v<allocator_type>)
            requires std::default_initializable<allocator_type> = default;

        constexpr explicit AllocationDeleter(const allocator_type& Allocator)
            noexcept(std::is_nothrow_copy_constructible_v<allocator_type>)
            : BaseType { Allocator }
        {
        }

        constexpr void operator()(pointer Target)
            noexcept(std::is_nothrow_destructible_v<typename std::allocator_traits<allocator_type>::value_type>)
        {
            DeleterAllocatorTraits::destroy(BaseType::Get(), std::to_address(Target));
            DeleterAllocatorTraits::deallocate(BaseType::Get(), Target, 1u);
        }
    };
}

#endif // ENGINE_SOURCES_MEMORY_FILE_DELETER_H
