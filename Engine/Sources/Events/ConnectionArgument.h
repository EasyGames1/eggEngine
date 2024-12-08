#ifndef ENGINE_SOURCES_EVENTS_FILE_CONNECTION_ARGUMENT_H
#define ENGINE_SOURCES_EVENTS_FILE_CONNECTION_ARGUMENT_H

namespace egg::Events
{
    template <auto>
    struct ConnectionArgumentType
    {
    };

    template <auto Candidate>
    inline constexpr ConnectionArgumentType<Candidate> ConnectionArgument {};
}

#endif // ENGINE_SOURCES_EVENTS_FILE_CONNECTION_ARGUMENT_H
