#ifndef ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_COMBINE_TUPLES_H
#define ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_COMBINE_TUPLES_H

#include <type_traits>

namespace egg::Types::Internal
{
    template <template<typename...> typename Tuple, typename... Tuples>
    struct CombineTuples;

    template <template<typename...> typename Tuple>
    struct CombineTuples<Tuple> : std::type_identity<Tuple<>>
    {
    };

    template <template<typename...> typename Tuple, typename... Types>
    struct CombineTuples<Tuple, Tuple<Types...>> : std::type_identity<Tuple<Types...>>
    {
    };

    template <template<typename...> typename Tuple, typename... LeftArgs, typename... RightArgs, typename... Other>
    struct CombineTuples<Tuple, Tuple<LeftArgs...>, Tuple<RightArgs...>, Other...>
        : CombineTuples<Tuple, Tuple<LeftArgs..., RightArgs...>, Other...>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_COMBINE_TUPLES_H
