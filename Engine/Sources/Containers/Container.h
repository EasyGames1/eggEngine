#ifndef ENGINE_SOURCES_CONTAINERS_FILE_CONTAINER_H
#define ENGINE_SOURCES_CONTAINERS_FILE_CONTAINER_H

#include <Types/Deduction/Deduction.h>

#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace egg::Containers
{
    template <typename ContainerType>
    [[nodiscard]] constexpr auto begin(ContainerType& Container) noexcept -> decltype(std::declval<ContainerType>().Begin())
    {
        return Container.Begin();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto begin(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().Begin())
    {
        return Container.Begin();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto cbegin(
        const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ConstBegin())
    {
        return Container.ConstBegin();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto end(ContainerType& Container) noexcept -> decltype(std::declval<ContainerType>().End())
    {
        return Container.End();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto end(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().End())
    {
        return Container.End();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto cend(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ConstEnd())
    {
        return Container.ConstEnd();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto rbegin(ContainerType& Container) noexcept -> decltype(std::declval<ContainerType>().ReverseBegin())
    {
        return Container.ReverseBegin();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto rbegin(
        const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ReverseBegin())
    {
        return Container.ReverseBegin();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto crbegin(
        const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ConstReverseBegin())
    {
        return Container.ConstReverseBegin();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto rend(ContainerType& Container) noexcept -> decltype(std::declval<ContainerType>().ReverseEnd())
    {
        return Container.ReverseEnd();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto rend(const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ReverseEnd())
    {
        return Container.ReverseEnd();
    }

    template <typename ContainerType>
    [[nodiscard]] constexpr auto crend(
        const ContainerType& Container) noexcept -> decltype(std::declval<const ContainerType>().ConstReverseEnd())
    {
        return Container.ConstReverseEnd();
    }

    //This Container.h is a helper for using for-range loops with custom containers whose naming convention differs from that used by std

    //Anything that isn't a free function that returns an iterator is a stub for #include unused warning

    //So other Container.h files is used to properly handle ADL. They're also redeclare some usings to stub #include unused warning

    //If the new container doesn't use anything declared in Container.h as stubs,
    //then you should extract anything like Traits, concept etc. into Container.h to prevent #include unused warnings

    template <typename Type>
    using AllocatorTraits = std::allocator_traits<Type>;

    template <typename Type>
    using IteratorTraits = std::iterator_traits<Type>;

    template <typename... Ts>
    using CommonTypeOf = Types::CommonTypeOf<Ts...>;
}

#endif // ENGINE_SOURCES_CONTAINERS_FILE_CONTAINER_H
