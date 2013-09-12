#ifndef SCOPED_LOCK_H
#define SCOPED_LOCK_H

#include "macros.h"
#include "normalize.h"

////////////////////////////////////////////////////////////////////////////////
// LockGuard

template<class Mutex>
class LockGuard {
  NOT_COPYABLE(LockGuard);

public:
  _always_inline explicit LockGuard(Mutex& m) :
    m_(m)
  {
    m_.lock();
  }

  _always_inline ~LockGuard() {
    m_.unlock();
  }

private:
  Mutex& m_;
};

////////////////////////////////////////////////////////////////////////////////
// ScopedLock

template<class Mutex>
class ScopedLock {
  NOT_COPYABLE(ScopedLock);
  MOVABLE(ScopedLock);

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

private:
  Mutex& m_;
  bool locked_;
};

#endif // SCOPED_LOCK_H
