#ifndef ENGINE_SOURCES_CONFIG_CONFIG_H
#define ENGINE_SOURCES_CONFIG_CONFIG_H

#ifndef EGG_ASSERT

#include <cassert>
#define EGG_ASSERT(Condition, Message) assert(Condition)

#endif

#define EGG_EVENTS_RESOLVE_MEMBER_FUNCTORS true

#endif // ENGINE_SOURCES_CONFIG_CONFIG_H
