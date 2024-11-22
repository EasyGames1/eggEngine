#ifndef ENGINE_SOURCES_TYPE_TRAITS_FILE_DEDUCTION_H
#define ENGINE_SOURCES_TYPE_TRAITS_FILE_DEDUCTION_H

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
}

#endif // ENGINE_SOURCES_TYPE_TRAITS_FILE_DEDUCTION_H
