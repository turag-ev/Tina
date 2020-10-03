#ifndef TINAPP_HELPER_SCOPED_LOCK_H
#define TINAPP_HELPER_SCOPED_LOCK_H

#include "normalize.h"

namespace TURAG {

/// \addtogroup Utils
/// \{

////////////////////////////////////////////////////////////////////////////////
// ScopedLock

/// \brief Lock für Mutexes
template<class Mutex>
class ScopedLock {
  ScopedLock(const ScopedLock&) = delete;
  ScopedLock& operator=(const ScopedLock&) = delete;

public:
  /// ScopedLock erstellen und Lock holen.
  TURAG_ALWAYS_INLINE explicit ScopedLock(Mutex& m) :
    m_(m), locked_(true)
  {
    m_.lock();
  }

  /// Lock verschieben
  TURAG_ALWAYS_INLINE explicit ScopedLock(ScopedLock&& other) :
	m_(other.m_), locked_(other.locked_)
  {
	other.locked_ = false;
  }

  /// Lock verschieben
  ScopedLock& operator = (ScopedLock&& other) {
	  unlock();

	  m_ = other.m_;
	  locked_ = other.locked_;
	  other.locked_ = false;
  }

  /// Lock u.U. freigeben und zerstören.
  TURAG_ALWAYS_INLINE ~ScopedLock() {
    unlock();
  }

  /// Lock freigeben, wenn Instanz Lock besitzt.
  TURAG_ALWAYS_INLINE void unlock() {
    if (locked_) {
      locked_ = false;
      m_.unlock();
    }
  }

  /// Lock holen, falls Instanz ihn noch nicht besitzt.
  TURAG_ALWAYS_INLINE void lock() {
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
