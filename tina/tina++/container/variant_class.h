#ifndef TINA_CONTAINER_VARIANT_CLASS_H
#define TINA_CONTAINER_VARIANT_CLASS_H

#include <cstdio>
#include <type_traits>

#include "array_storage.h"

namespace TURAG {

namespace detail { namespace {

template<class Abstract, typename...Tail>
struct are_base_of_impl : std::true_type { };

template<class Abstract, typename Head, typename...Tail>
struct are_base_of_impl<Abstract, Head, Tail...> :
  std::integral_constant<bool, std::is_base_of<Abstract, Head>::value && are_base_of_impl<Abstract, Tail...>::value>
{ };

template<class Abstract, class ...Variants>
struct are_base_of : are_base_of_impl<Abstract, Variants...> { };

}}
 
template<class Abstract, class ...Variants,
  std::enable_if<all_true<is_trival_destructable<Variants>::value...> >,
  std::enable_if<all_true<is_trival_copyable<Variants>::value...> > >
class VariantClass {
  static_assert(are_base_of<Abstract, Variants...>::value, "Nicht alle Klassen sind Implemtierungen von ersten Templateparameter");

public:
  constexpr
  VariantClass() :
    data_(), active_(nullptr)
  { }
  
  constexpr
  bool empty() const {
    return active_ == nullptr;
  }
  
  explicit constexpr
  operator bool () const {
    return active_ != nullptr;
  }

  Abstract* operator &() {
    return active_;
  }
  
  constexpr
  const Abstract* operator &() const {
    return active_;
  }
  
  Abstract* operator -> () {
    return active_;
  }
 
  constexpr
  const Abstract* operator -> () const {
    return active_;
  }
  
  template<typename T, typename ...Args>
  void emplace(Args&&... args) {
    construct(reinterpret_cast<T*>(&data_), std::forward<Args>(args)...);
    active_ = static_cast<Abstract*>(reinterpret_cast<T*>(&data_));
  }

private:
  union_storage<Variants...> data_;
  Abstract* active_;  
  // std::size_t index;
};

} // namespace TURAG

#endif // TINA_CONTAINER_VARIANT_CLASS_H
