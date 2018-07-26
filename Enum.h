#include <type_traits>

#include "ForEachMacro.h"

#define STRINGIFY_(...) #__VA_ARGS__
#define STRINGIFY(...) STRINGIFY_(__VA_ARGS__)

#define ENUM_CASE_TO_STRING(X)                                   \
    case X:                                                      \
        return #X;

// Effectively creates an "enum class" with easy access to printing
// and getting the underlying value. The main limitation is that
// you cannot specify any of the value, rather you must accept the
// default values provided for each member in the enum.
#define ENUM(EnumClassT, UnderlyingType, ...)                    \
    struct EnumClassT  {                                         \
	static_assert(std::is_integral<UnderlyingType>::value,   \
                      "UnderlyingType must be an integer.");     \
                                                                 \
        enum Enum : UnderlyingType {                             \
            __VA_ARGS__                                          \
        };                                                       \
                                                                 \
        std::string ToString() {                                 \
            switch (val_) {                                      \
                FOREACH(ENUM_CASE_TO_STRING, (__VA_ARGS__))      \
            }                                                    \
        }                                                        \
                                                                 \
        EnumClassT(Enum e) : val_(e) {}                          \
                                                                 \
        UnderlyingType value() {                                 \
            return val_;                                         \
        }                                                        \
                                                                 \
        Enum val_;                                               \
    }; struct __SEMICOLON

