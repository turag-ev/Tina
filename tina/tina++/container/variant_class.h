#ifndef TINA_CONTAINER_VARIANT_CLASS_H
#define TINA_CONTAINER_VARIANT_CLASS_H

#include <cstdio>
#include <type_traits>

#include <tina++/helper/traits_algorithms.h>
#include "array_storage.h"

namespace TURAG {

template<class Abstract, class ...Variants>
class VariantClass {
	static_assert(all_true<std::is_base_of<Abstract, Variants>...>::value, "Nicht alle Klassen sind Implemtierungen von ersten Templateparameter");
    //static_assert(all_true<std::is_trivially_copyable<Variants>...>::value, "FIXME");

public:

	// cdtors

	constexpr
	VariantClass() :
	data_(), active_(nullptr)
	{ }

	~VariantClass() {
		erase();
	}

    template<class T>
    VariantClass(const T& rhs):
        active_(reinterpret_cast<Abstract*>(data_) )
    {
        static_assert(!all_false<std::is_same<T,Variants>...>::value, "Can't construct from non-variant class.");
        construct(reinterpret_cast<T*>(data_), rhs);
    }

    VariantClass(const VariantClass<Abstract, Variants...>& rhs)
    {
        memcpy(data_, rhs.data_, max_value(sizeof(Variants)...) );
        if(rhs.empty())
            active_ = nullptr;
        else
            active_ = reinterpret_cast<Abstract*>(data_);
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
        construct(reinterpret_cast<T*>(data_), std::forward<Args>(args)...);
        active_ = static_cast<Abstract*>(reinterpret_cast<T*>(data_));
        //assert(active_ != nullptr);
	}

private:
    unsigned char data_[max_value(sizeof(Variants)...)];
    Abstract* active_;
};

} // namespace TURAG

#endif // TINA_CONTAINER_VARIANT_CLASS_H
