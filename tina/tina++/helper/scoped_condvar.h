#ifndef TINAPP_HELPER_SCOPED_CONDVAR_H
#define TINAPP_HELPER_SCOPED_CONDVAR_H

#include "scoped_lock.h"

namespace TURAG {

template<class ConditionVariable>
class ScopedConditionVariable : ScopedLock<ConditionVariable::Mutex> {
public:
    ScopedConditionVariable(ConditionVariable& condvar) :
        ScopedLock<ConditionVariable::Mutex>(condvar.getMutex()),
        condvar_(condvar)
    { }

    _always_inline bool wait() {
        return condvar_.wait();
    }

    _always_inline bool waitFor(SystemTime timeout) {
        return condvar_.waitFor(timeout);
    }

    _always_inline bool waitUntil(SystemTime timeout) {
        return condvar_.waitUntil(timeout);
    }

private:
    ConditionVariable& condvar_;
};

} // namespace TURAG

#endif // TINAPP_HELPER_SCOPED_CONDVAR_H
