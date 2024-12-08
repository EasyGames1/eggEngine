#ifndef ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_MEMBER_OBJECT_POINTER_TRAITS_H
#define ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_MEMBER_OBJECT_POINTER_TRAITS_H

namespace egg::Types::Internal
{
    template <typename>
    struct MemberObjectPointerTraits;

    template <typename Type, typename Class>
    struct MemberObjectPointerTraits<Type Class::*>
    {
        using Member = Type;
        using Instance = Class;
    };
}

#endif // ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_MEMBER_OBJECT_POINTER_TRAITS_H
