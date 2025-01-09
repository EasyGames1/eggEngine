#ifndef ENGINE_SOURCES_TYPES_FILE_DEDUCTION_H
#define ENGINE_SOURCES_TYPES_FILE_DEDUCTION_H

#include "./Internal/CombineTuples.h"
#include "./Internal/CommonTypeOf.h"
#include "./Internal/FilterTuple.h"
#include "./Internal/InstantiateTuple.h"
#include "./Internal/MemberObjectPointerTraits.h"
#include "./Internal/RemoveTupleIndex.h"
#include "./Internal/RemoveTupleType.h"
#include "./Internal/TupleSize.h"
#include "./Internal/TypeIndexIn.h"

namespace egg::Types
{
    template <typename MemberObjectPointer> requires std::is_member_object_pointer_v<MemberObjectPointer>
    using MemberObjectPointerTraits = Internal::MemberObjectPointerTraits<MemberObjectPointer>;

    template <typename Type, typename Tuple>
    using RemoveTupleType = typename Internal::RemoveTupleType<Type, Tuple>::type;

    template <std::size_t Index, typename Tuple>
    using RemoveTupleIndex = typename Internal::RemoveTupleIndex<Index, Tuple>::type;

    template <template<typename...> typename Tuple, typename... Tuples>
    using CombineTuples = typename Internal::CombineTuples<Tuple, Tuples...>::type;

    template <template<typename> typename PredicateType, typename Tuple>
    using FilterTuple = typename Internal::FilterTuple<PredicateType, Tuple>::type;

    template <template<typename> typename Template, typename Tuple>
    using InstantiateTuple = typename Internal::InstantiateTuple<Template, Tuple>::type;

    template <typename... Types>
    using CommonTypeOf = typename Internal::CommonTypeOf<Types...>::type;

    template <typename Tuple>
    using CommonTypeOfTuple = typename Internal::CommonTypeOfTuple<Tuple>::type;

    template <typename Type, typename... Types>
    inline constexpr std::size_t TypeIndexIn = Internal::TypeIndexIn<Type, Types...>::value;

    template <typename Type, typename Tuple>
    inline constexpr std::size_t TypeIndexInTuple = Internal::TypeIndexInTuple<Type, Tuple>::value;

    template <typename Tuple>
    inline constexpr std::size_t TupleSize = Internal::TupleSize<Tuple>::value;
}

#endif // ENGINE_SOURCES_TYPES_FILE_DEDUCTION_H
