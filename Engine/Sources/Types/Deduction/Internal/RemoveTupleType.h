#ifndef ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_REMOVE_ELEMENT_H
#define ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_REMOVE_ELEMENT_H

#include <tuple>
#include <type_traits>

namespace egg::Types::Internal
{
    template <typename, typename>
    struct RemoveTupleType;

    template <typename Type, template<typename...> typename Tuple>
    struct RemoveTupleType<Type, Tuple<>> : std::type_identity<Tuple<>>
    {
    };

    template <typename Type, template<typename...> typename Tuple, typename... Types>
    struct RemoveTupleType<Type, Tuple<Type, Types...>>
        : RemoveTupleType<Type, Tuple<Types...>>
    {
    };

    template <typename Type, template<typename...> typename Tuple, typename Other, typename... Types>
    struct RemoveTupleType<Type, Tuple<Other, Types...>>
        : CombineTuples<Tuple, Tuple<Other>, typename RemoveTupleType<Type, Types...>::type>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_REMOVE_ELEMENT_H
