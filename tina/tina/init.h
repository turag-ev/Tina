#ifndef INIT_H
#define INIT_H

#include "macros.h"
#include "normalize.h"

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// initializer (startup code)

/// intialize variables or array elements from everywhere in the code
/// outside of function or method blocks the variable is initialized before main function is called (called when all ctors of static classes are called).
/// inside of function or method blocks the variable is initialized when function or method is called the first time.
/// use: \code static initializer one(variable, new_value); \endcode
class initializer {
public:
  template<typename T>
  _always_inline initializer(T& var, T value) {
    var = value;
  }
};

/// macro for easier use for initializer class
#define INIT_VAR(v,x) static initializer CONCAT(_initializer_,__COUNTER__) ((v), (x))

/// Call function at initialization
/// outside of function or method blocks the function is called before main function is called (called when all ctors of static classes are called).
/// inside of function or method blocks the function is called when function or method is called the first time.
/// use: \code static initializer_call one(function_name); \endcode
class initializer_call {
public:
  template<typename F>
  _always_inline initializer_call(F func) {
    func();
  }
};

/// macro for call a function initialization
#define INIT_CALL(v) static initializer_call CONCAT(_initializer_,__COUNTER__) (v)

} // namespace TURAG

#endif // INIT_H
