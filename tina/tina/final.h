#ifndef FINAL_H
#define FINAL_H

#include "macros.h"
#include "normalize.h"

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// finalizer (exit code)

/// Call function at finalization
/// function is called when program exists.
/// use: \code static finalizer_call one(function_name); \endcode
template<typename Function>
class finalizer_call {
public:
  _always_inline finalizer_call(Function function) :
    function_(function)
  { }

  _always_inline ~finalizer_call() {
    function_();
  }

private:
  Function function_;
};

/// macro for call a function initialization
#define FINAL_CALL(v) static finalizer_call CONCAT(_finalizer_,__COUNTER__) (v)

} // namespace TURAG

#endif // FINAL_H
