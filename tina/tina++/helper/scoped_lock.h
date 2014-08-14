#ifndef TINAPP_HELPER_SCOPED_LOCK_H
#define TINAPP_HELPER_SCOPED_LOCK_H

#include "macros.h"
#include "normalize.h"

namespace TURAG {

/// \addtogroup Utils
/// \{

////////////////////////////////////////////////////////////////////////////////
// ScopedLock

/// \brief Lock für Mutexes
template<class Mutex>
class ScopedLock {
  NOT_COPYABLE(ScopedLock);
  NOT_MOVABLE(ScopedLock);

public:
  /// ScopedLock erstellen und Lock holen.
  _always_inline explicit ScopedLock(Mutex& m) :
    m_(m), locked_(true)
  {
    m_.lock();
  }

  /// Lock u.U. freigeben und zerstören.
  _always_inline ~ScopedLock() {
    unlock();
  }

  /// Lock freigeben, wenn Instanz Lock besitzt.
  _always_inline void unlock() {
    if (locked_) {
      locked_ = false;
      m_.unlock();
    }
  }

  /// Lock holen, falls Instanz ihn noch nicht besitzt.
  _always_inline void lock() {
    if (!locked_) {
      locked_ = true;
      m_.lock();
    }
  }

  /// Besitzzustand zurückgeben
  /// \returns \a true, wenn Instanz Lock besitzt, sonst \a false.
  bool owns() const {
    return locked_;
  }

  /// Besitzzustand zurückgeben
  /// \returns \a true, wenn Instanz Lock besitzt, sonst \a false.
  explicit
  operator bool () const {
    return locked_;
  }

  /// zugehörigen Mutex zurückgeben
  Mutex& getMutex() { return m_; }

private:
  /// zugehöriger Mutex
  Mutex& m_;

  /// Besitzzustand
  bool locked_;
};

/// \}

} // namespace TURAG

#endif // TINAPP_HELPER_SCOPED_LOCK_H
