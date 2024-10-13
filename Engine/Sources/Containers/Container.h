#ifndef ENGINE_SOURCES_CONTAINERS_FILE_CONTAINER_H
#define ENGINE_SOURCES_CONTAINERS_FILE_CONTAINER_H

#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace egg::Containers
{
    template <typename ContainerType>
    [[nodiscard]] auto begin(ContainerType& Container) noexcept -> decltype(std::declval<ContainerType>().Begin())
    {
        return Container.Begin();
    }

    template <typename ContainerType>
    [[nodiscard]] auto begin(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().Begin())
    {
        return Container.Begin();
    }

    template <typename ContainerType>
    [[nodiscard]] auto cbegin(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ConstBegin())
    {
        return Container.ConstBegin();
    }

    template <typename ContainerType>
    [[nodiscard]] auto end(ContainerType& Container) noexcept -> decltype(std::declval<ContainerType>().End())
    {
        return Container.End();
    }

    template <typename ContainerType>
    [[nodiscard]] auto end(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().End())
    {
        return Container.End();
    }

    template <typename ContainerType>
    [[nodiscard]] auto cend(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ConstEnd())
    {
        return Container.ConstEnd();
    }

    template <typename ContainerType>
    [[nodiscard]] auto rbegin(ContainerType& Container) noexcept -> decltype(std::declval<ContainerType>().ReverseBegin())
    {
        return Container.ReverseBegin();
    }

    template <typename ContainerType>
    [[nodiscard]] auto rbegin(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ReverseBegin())
    {
        return Container.ReverseBegin();
    }

    template <typename ContainerType>
    [[nodiscard]] auto crbegin(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ConstReverseBegin())
    {
        return Container.ConstReverseBegin();
    }

    template <typename ContainerType>
    [[nodiscard]] auto rend(ContainerType& Container) noexcept -> decltype(std::declval<ContainerType>().ReverseEnd())
    {
        return Container.ReverseEnd();
    }

    template <typename ContainerType>
    [[nodiscard]] auto rend(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ReverseEnd())
    {
        return Container.ReverseEnd();
    }

    template <typename ContainerType>
    [[nodiscard]] auto crend(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ConstReverseEnd())
    {
        return Container.ConstReverseEnd();
    }

    //This Container.h is a helper for using for-range loops with custom containers whose naming convention differs from that used by std

    //Anything that isn't a free function that returns an iterator is a stub for #include unused warning

    //So other Container.h files is used to properly handle ADL. They're also redeclare some concepts to stub #include unused warning

    //If the new container doesn't use anything declared in Container.h as stubs,
    //then you should extract anything like Traits, concept etc. into Container.h to prevent #include unused warnings

    template <typename Type>
    using AllocatorTraits = std::allocator_traits<Type>;

    template <typename Type>
    using IteratorTraits = std::iterator_traits<Type>;

    template <typename AllocatorType, typename Type>
    concept ValidAllocator = std::is_same_v<Type, typename AllocatorTraits<AllocatorType>::value_type>;
}

#endif // ENGINE_SOURCES_CONTAINERS_FILE_CONTAINER_H
