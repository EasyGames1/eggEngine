#ifndef ENGINE_SOURCES_TYPES_FILE_CONSTNESS_H
#define ENGINE_SOURCES_TYPES_FILE_CONSTNESS_H

#include <type_traits>

namespace egg::Types
{
    template <typename From, typename To>
    using ConstnessAs = std::conditional_t<std::is_const_v<From>, std::add_const_t<To>, To>;

    template <typename From, typename To>
    using ConstnessAsPointee = ConstnessAs<std::remove_pointer_t<From>, To>;

    template <typename From, typename To>
    using PointeeConstnessAsOther = std::add_pointer_t<ConstnessAsPointee<From, To>>;
}

#endif // ENGINE_SOURCES_TYPES_FILE_CONSTNESS_H