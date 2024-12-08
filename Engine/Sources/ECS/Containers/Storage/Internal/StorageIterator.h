#ifndef ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_INTERNAL_FILE_STORAGE_ITERATOR_H
#define ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_INTERNAL_FILE_STORAGE_ITERATOR_H

#include <Containers/PointerImitator.h>

#include <iterator>
#include <tuple>

namespace egg::ECS::Containers::Internal
{
    template <typename EntityIteratorParameter, typename ElementIteratorParameter>
    class StorageIterator final
    {
    public:
        using EntityIteratorType = EntityIteratorParameter;
        using ElementIteratorType = ElementIteratorParameter;
        using value_type = std::tuple<std::iter_value_t<EntityIteratorType>,
                                      std::add_rvalue_reference_t<std::iter_reference_t<ElementIteratorType>>>;
        using pointer = egg::Containers::PointerImitator<value_type>;
        using reference = value_type;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;


        constexpr StorageIterator() : EntityIterator {}, ElementIterator {}
        {
        }

        constexpr explicit StorageIterator(EntityIteratorType EntityIterator, ElementIteratorType ElementIterator)
            : EntityIterator { EntityIterator }, ElementIterator { ElementIterator }
        {
        }

        template <std::convertible_to<ElementIteratorType> OtherIterator> requires (!std::same_as<OtherIterator, ElementIteratorType>)
        constexpr explicit StorageIterator(const StorageIterator<EntityIteratorType, OtherIterator>& Other)
            : EntityIterator { Other.EntityIterator }, ElementIterator { Other.ElementIterator }
        {
        }

        constexpr StorageIterator& operator++() noexcept
        {
            ++EntityIterator;
            ++ElementIterator;
            return *this;
        }

        constexpr StorageIterator operator++(int) noexcept
        {
            StorageIterator Original { *this };
            ++*this;
            return Original;
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept
        {
            return pointer { operator*() };
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
            return { *EntityIterator, *ElementIterator };
        }

        template <std::convertible_to<ElementIteratorType> OtherIterator>
        [[nodiscard]] constexpr bool operator==(const StorageIterator<EntityIteratorType, OtherIterator>& Other) const noexcept
        {
            return EntityIterator == Other.EntityIterator;
        }

        template <std::convertible_to<ElementIteratorType> OtherIterator>
        [[nodiscard]] constexpr bool operator!=(const StorageIterator<EntityIteratorType, OtherIterator>& Other) const noexcept
        {
            return !(*this == Other);
        }

    private:
        EntityIteratorType EntityIterator;
        ElementIteratorType ElementIterator;
    };
}

#endif // ENGINE_SOURCES_ECS_CONTAINERS_STORAGE_INTERNAL_FILE_STORAGE_ITERATOR_H
