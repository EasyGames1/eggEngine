#ifndef ENGINE_SOURCES_TYPE_TRAITS_FILE_DEDUCTION_H
#define ENGINE_SOURCES_TYPE_TRAITS_FILE_DEDUCTION_H

#include "./Internal/HasType.h"

namespace egg::Types
{
    template <typename>
    struct MemberObjectTraits;

    template <typename Type, typename Class>
    struct MemberObjectTraits<Type Class::*>
    {
        using Member = Type;
        using Instance = Class;
    };

    template <typename Type, typename Tuple>
    concept ContainedIn = Internal::HasType<Type, Tuple>::value;
}

#endif // ENGINE_SOURCES_TYPE_TRAITS_FILE_DEDUCTION_H
