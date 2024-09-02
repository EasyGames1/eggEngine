#ifndef ENGINE_SOURCES_CONFIG_CONFIG_H
#define ENGINE_SOURCES_CONFIG_CONFIG_H

#ifndef EGG_ASSERT

#include <cassert>
#define EGG_ASSERT(Condition, Message) assert(Condition)

#endif

#include <string_view>

namespace egg::TypeInfo
{
#if defined __clang__ || defined __GNUC__

#define EGG_PRETTY_FUNCTION __PRETTY_FUNCTION__
    inline constexpr char PrettyFunctionPrefix { '=' };
    inline constexpr std::string_view PrettyFunctionSuffix { "];" };

#elif defined _MSC_VER

#define EGG_PRETTY_FUNCTION __FUNCSIG__
    inline constexpr char PrettyFunctionPrefix { '<' };
    inline constexpr char PrettyFunctionSuffix { '>' };

#endif

#ifdef EGG_PRETTY_FUNCTION
    inline constexpr bool IsPrettyFunction { true };
#else
    inline constexpr bool IsPrettyFunction { false };
#endif
}

#endif // ENGINE_SOURCES_CONFIG_CONFIG_H
