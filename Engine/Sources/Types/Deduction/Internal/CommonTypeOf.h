#ifndef ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_COMMON_TYPE_OF_H
#define ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_COMMON_TYPE_OF_H

#include <type_traits>

namespace egg::Types::Internal
{
    template <typename... Types>
    using CommonTypeOf = std::common_type<Types...>;

    template <typename>
    struct CommonTypeOfTuple;

    template <template<typename...> typename Tuple, typename... Types>
    struct CommonTypeOfTuple<Tuple<Types...>> : CommonTypeOf<Types...>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_COMMON_TYPE_OF_H
