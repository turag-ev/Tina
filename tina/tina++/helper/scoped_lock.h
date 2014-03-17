#ifndef TINAPP_HELPER_SCOPED_LOCK_H
#define TINAPP_HELPER_SCOPED_LOCK_H

#include "macros.h"
#include "normalize.h"

namespace TURAG {

/// \addtogroup Utils
/// \{

////////////////////////////////////////////////////////////////////////////////
// ScopedLock

template<class Mutex>
class ScopedLock {
  NOT_COPYABLE(ScopedLock);
  NOT_MOVABLE(ScopedLock);

public:
  _always_inline explicit ScopedLock(Mutex& m) :
    m_(m), locked_(true)
  {
    m_.lock();
  }

  _always_inline ~ScopedLock() {
    if (locked_) {
      m_.unlock();
    }
  }

  _always_inline void unlock() {
    if (locked_) {
      locked_ = false;
      m_.unlock();
    }
  }

  _always_inline void lock() {
    if (!locked_) {
      locked_ = true;
      m_.lock();
    }
  }

  bool owns() const {
    return locked_;
  }

  explicit
  operator bool () const {
    return locked_;
  }

  Mutex& getMutex() { return m_; }

private:
  Mutex& m_;
  bool locked_;
};

/// \}

} // namespace TURAG

#endif // TINAPP_HELPER_SCOPED_LOCK_H
