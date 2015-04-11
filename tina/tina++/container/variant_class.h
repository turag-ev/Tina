#ifndef TINA_CONTAINER_VARIANT_CLASS_H
#define TINA_CONTAINER_VARIANT_CLASS_H

#include <cstdio>
#include <type_traits>

#include <tina++/helper/traits_algorithms.h>
#include "array_storage.h"

namespace TURAG {

template<class Abstract, class ...Variants>
class VariantClass final {
	//static_assert(all_true<std::is_trivially_copyable<Variants>...>::value, "FIXME");
	static_assert(all_true<std::is_base_of<Abstract, Variants>...>::value, "Nicht alle Klassen sind Implemtierungen von ersten Templateparameter");

public:

	// cdtors

	constexpr
	VariantClass() :
	data_(), active_(nullptr)
	{ }

	~VariantClass() {
		erase();
	}

	// empty

	constexpr
	bool empty() const {
		return active_ == nullptr;
	}

	explicit constexpr
	operator bool () const {
		return !empty();
	}

	// get pointer

	Abstract* get() {
		return active_;
	}

	constexpr
	const Abstract* get() const {
		return active_;
	}

	Abstract* operator -> () {
		return active_;
	}

	constexpr
	const Abstract* operator -> () const {
		return active_;
	}

	// get reference

	Abstract& operator * () {
		return *active_;
	}

	constexpr
	const Abstract& operator * () const {
		return *active_;
	}

	// modify

	void erase() {
		if (empty()) return;

		Abstract* old_ptr = active_;
		active_ = nullptr;

		destruct(old_ptr);
	}

	template<typename T, typename ...Args>
	void emplace(Args&&... args) {
		static_assert(!all_false<std::is_same<T, Variants>...>::value,
				"Der Templateparamter ist keiner der speicherbaren Typen. "
				"Typ muss zu VariantClass-Definition als Parameter hinzugefügt werden.");
		erase();
		construct(reinterpret_cast<T*>(&data_), std::forward<Args>(args)...);
		active_ = static_cast<Abstract*>(reinterpret_cast<T*>(&data_));
		assert(active_ != nullptr);
	}

private:
	UnionStorage<Variants...> data_;
	Abstract* active_;
};

} // namespace TURAG

#endif // TINA_CONTAINER_VARIANT_CLASS_H
