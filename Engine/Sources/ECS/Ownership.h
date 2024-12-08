#ifndef ENGINE_SOURCES_ECS_FILE_OWNERSHIP_H
#define ENGINE_SOURCES_ECS_FILE_OWNERSHIP_H

namespace egg::ECS
{
    template <typename... Type>
    struct OwnType final
    {
    };


    template <typename... Type>
    struct ViewType final
    {
    };


    template <typename... Type>
    struct ExcludeType final
    {
    };


    template <typename... Type>
    inline constexpr OwnType<Type...> Own {};

    template <typename... Type>
    inline constexpr ViewType<Type...> View {};

    template <typename... Type>
    inline constexpr ExcludeType<Type...> Exclude {};
}

#endif // ENGINE_SOURCES_ECS_FILE_OWNERSHIP_H
