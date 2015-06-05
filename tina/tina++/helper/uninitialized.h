#ifndef UNINITIALIZED_H
#define UNINITIALIZED_H

#include <utility>
#include <new>

#include "macros.h"
#include "construct.h"
#include "../container/array_storage.h"

namespace TURAG {

template<typename T>
class uninitialized {
private:
public:
  constexpr uninitialized() { }

  ~uninitialized() {
    get().~T();
  }

  explicit uninitialized(const uninitialized& other) {
    construct(other);
  }

  explicit uninitialized(uninitialized&& other) {
    construct(other);
  }

  template<class... Args>
  explicit uninitialized(Args&&... args) {
    construct(std::forward<Args>(args)...);
  }

  template<class... Args>
  void construct(Args&&... args) {
    ::new(getPointer()) T (std::forward<Args>(args)...);
  }

  uninitialized& operator = (const T& t) {
    get() = t;
    return *this;
  }

  uninitialized& operator = (T&& t) {
    get() = std::move(t);
    return *this;
  }

  T* operator -> () { return getPointer(); }
  T& operator * () { return get(); }
  T* operator & () { return getPointer(); }
  T* getPointer() { return reinterpret_cast<T*>(&data); }
  T& get() { return *reinterpret_cast<T*>(&data); }

  const T* operator -> () const { return getPointer(); }
  const T& operator * () const { return get(); }
  const T* operator & () const { return getPointer(); }
  const T* getPointer() const { return reinterpret_cast<const T*>(&data); }
  const T& get() const { return *reinterpret_cast<const T*>(&data); }

private:
  TypeStorage<T> data;
};

} // namespace TURAG

#endif // UNINITIALIZED_H
