#ifndef ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_INSTANTIATE_TUPLE_H
#define ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_INSTANTIATE_TUPLE_H

#include <type_traits>

namespace egg::Types::Internal
{
    template <template<typename> typename, typename>
    struct InstantiateTuple;

    template <template<typename> typename Template, template<typename...> typename Tuple, typename... Types>
    struct InstantiateTuple<Template, Tuple<Types...>> : std::type_identity<Tuple<Template<Types>...>>
    {
    };
}

#endif // ENGINE_SOURCES_TYPES_DEDUCTION_INTERNAL_FILE_INSTANTIATE_TUPLE_H
