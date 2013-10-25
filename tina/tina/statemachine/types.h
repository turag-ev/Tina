#ifndef TINA_STATEMACHINE_TYPES_H
#define TINA_STATEMACHINE_TYPES_H

namespace TURAG {

/// type for event ids
typedef uint32_t EventId;

/// function for event processing
typedef void (*EventMethod)(EventId id, pointer data);

/// create a unsigned integer with the first three bytes filled with characters
/// to build a namespace. The last byte can be used for any value >= 0 and < 256.
/// example:
/// \code enum { event_x = EventNamespace('X', 'X', 'X') + 100 }; \endcode
constexpr
int32_t EventNamespace(char a, char b, char c) {
  return (a << 24) | (b << 16) | (c << 8);
}

} // namespace TURAG


#endif // TINA_STATEMACHINE_TYPES_H
