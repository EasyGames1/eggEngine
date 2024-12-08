#ifndef ENGINE_SOURCES_CONTAINERS_FILE_ITERABLE_ADAPTOR_H
#define ENGINE_SOURCES_CONTAINERS_FILE_ITERABLE_ADAPTOR_H

#include <Containers/Container.h>

#include <iterator>

namespace egg::Containers
{
    template <typename IteratorType, std::sentinel_for<IteratorType> SentinelType = IteratorType>
    class IterableAdaptor final
    {
    public:
        using ValueType = typename IteratorTraits<IteratorType>::value_type;

        using Iterator = IteratorType;
        using ReverseIterator = std::reverse_iterator<Iterator>;

        using Sentinel = SentinelType;
        using ReverseSentinel = std::reverse_iterator<Sentinel>;

        constexpr IterableAdaptor()
            noexcept (std::is_nothrow_default_constructible_v<Iterator> && std::is_nothrow_default_constructible_v<Sentinel>)
            : First {}, Last {}
        {
        }

        constexpr IterableAdaptor(Iterator From, Sentinel To)
            noexcept (std::is_nothrow_move_constructible_v<Iterator> && std::is_nothrow_move_constructible_v<Sentinel>)
            : First { std::move(From) }, Last { std::move(To) }
        {
        }

        [[nodiscard]] constexpr Iterator Begin() const noexcept
        {
            return First;
        }

        [[nodiscard]] constexpr Sentinel End() const noexcept
        {
            return Last;
        }

    private:
        Iterator First;
        Sentinel Last;
    };
}

#endif // ENGINE_SOURCES_CONTAINERS_FILE_ITERABLE_ADAPTOR_H
