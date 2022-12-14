#ifndef TINAPP_HELPER_INIT_H
#define TINAPP_HELPER_INIT_H

#include <memory>

#include "normalize.h"

namespace TURAG {
/// \addtogroup Utils
/// \{

////////////////////////////////////////////////////////////////////////////////
// initializer (startup code)

/// intialize variables or array elements from everywhere in the code
/// outside of function or method blocks the variable is initialized before main function is called (called when all ctors of static classes are called).
/// inside of function or method blocks the variable is initialized when function or method is called the first time.
/// use: \code static initializer one(variable, new_value); \endcode
class initializer {
public:
  template<typename T>
  TURAG_ALWAYS_INLINE initializer(T& var, T value) {
    var = value;
  }
};

/// macro for easier use for initializer class
#define TURAG_INIT_VAR(v,x) static initializer TURAG_CONCAT(turag_initializer_,__COUNTER__) ((v), (x))

/// Call function at initialization
/// outside of function or method blocks the function is called before main function is called (called when all ctors of static classes are called).
/// inside of function or method blocks the function is called when function or method is called the first time.
/// use: \code static initializer_call one(function_name); \endcode
class initializer_call {
public:
  template<typename F, typename... Args>
  TURAG_ALWAYS_INLINE initializer_call(F func, Args&&... args) {
      func(std::forward<Args>(args)...);
  }
};

/// macro for call a function initialization
#define TURAG_INIT_CALL(v, ...) static initializer_call TURAG_CONCAT(turag_initializer_,__COUNTER__) (v, ## __VA_ARGS__)

/// \}

} // namespace TURAG

#endif // TINAPP_HELPER_INIT_H
