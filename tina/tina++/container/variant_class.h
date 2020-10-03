#ifndef TINAPP_CONTAINER_VARIANT_CLASS_H
#define TINAPP_CONTAINER_VARIANT_CLASS_H

#include <cassert>
#include <type_traits>

#include <tina++/helper/traits_algorithms.h>
#include "array_storage.h"

namespace TURAG {

namespace detail {

template<typename>
void destruct_as_one_of(void*, const std::size_t) {
    // This is reached if idx >= sizeof...(Variants)
}

template<typename, typename T0, typename... Ts>
void destruct_as_one_of(void* obj, const std::size_t idx) {
    if (idx) {
        destruct_as_one_of<void, Ts...>(obj, idx - 1);
    } else {
        destruct(static_cast<T0*>(obj));
    }
}

template<typename Base>
Base* copy_construct_as_one_of(void*, const Base&, const std::size_t) {
    // This is reached if idx >= sizeof...(Variants)
    return nullptr;
}

template<typename Base, typename T0, typename... Ts>
Base* copy_construct_as_one_of(void* target, const Base& source, const std::size_t idx) {
    if (idx) {
        return copy_construct_as_one_of<Base, Ts...>(target, source, idx - 1);
    } else {
        construct(reinterpret_cast<T0*>(target), static_cast<const T0&>(source));
        return reinterpret_cast<T0*>(target);
    }
}

template<typename Base>
Base* move_construct_as_one_of(void*, Base&&, const std::size_t) {
    // This is reached if idx >= sizeof...(Variants)
    return nullptr;
}

template<typename Base, typename T0, typename... Ts>
Base* move_construct_as_one_of(void* target, Base&& source, const std::size_t idx) {
    if (idx) {
        return move_construct_as_one_of<Base, Ts...>(target, std::forward<Base>(source), idx - 1);
    } else {
        construct(reinterpret_cast<T0*>(target), std::forward<T0>(static_cast<T0&&>(source)));
        return reinterpret_cast<T0*>(target);
    }
}

template<typename Base>
Base* cast_as_one_of(void*, const std::size_t) {
    // This is reached if idx >= sizeof...(Variants)
    return nullptr;
}

template<typename Base, typename T0, typename... Ts>
Base* cast_as_one_of(void* obj, const std::size_t idx) {
    if (idx) {
        return cast_as_one_of<Base, Ts...>(obj, idx - 1);
    } else {
        return reinterpret_cast<T0*>(obj);
    }
}

template<typename Base>
const Base* cast_as_one_of(const void*, const std::size_t) {
    // This is reached if idx >= sizeof...(Variants)
    return nullptr;
}

template<typename Base, typename T0, typename... Ts>
const Base* cast_as_one_of(const void* obj, const std::size_t idx) {
    if (idx) {
        return cast_as_one_of<Base, Ts...>(obj, idx - 1);
    } else {
        return reinterpret_cast<const T0*>(obj);
    }
}

template<typename T>
constexpr std::size_t index_of_type(const std::size_t idx = 0) {
    // This is reached if idx >= sizeof...(Variants)
    return idx;
}

template<typename T, typename T0, typename... Ts>
constexpr std::size_t index_of_type(const std::size_t idx = 0) {
    return std::is_same<T, T0>::value ? idx : index_of_type<T, Ts...>(idx + 1);
}

} // namespace detail


template<class Abstract, class... Variants>
class VariantClass {
	static_assert(all_true<std::is_base_of<Abstract, Variants>...>::value, "Nicht alle Klassen sind Implementierungen des ersten Templateparameters");
public:

	// cdtors

	constexpr
	VariantClass() :
	data_(), active_idx_(sizeof... (Variants)) // initialize with invalid value.
	{ }

	~VariantClass()
	{
		erase();
	}

    template<class T>
    VariantClass(const T& rhs) : VariantClass()
    {
        emplace<T>(rhs);
	}

	VariantClass(const VariantClass& rhs) : VariantClass()
    {
        if (rhs) {
		    detail::copy_construct_as_one_of<Abstract, Variants...>(&data_, *rhs, rhs.active_idx_);
            active_idx_ = rhs.active_idx_;
        }
	}

	VariantClass(VariantClass&& rhs) : VariantClass()
    {
        if (rhs) {
            detail::move_construct_as_one_of<Abstract, Variants...>(&data_, std::forward<Abstract>(*rhs), rhs.active_idx_);
            active_idx_ = rhs.active_idx_;
        }
	}

	VariantClass operator=(VariantClass rhs)
	{
        // copy-and-swap idiom
        std::swap(data_, rhs.data_);
        std::swap(active_idx_, rhs.active_idx_);
		return *this;
    }

    // empty
	constexpr
	bool empty() const {
		return active_idx_ >= sizeof... (Variants);
	}

	explicit constexpr
	operator bool () const {
		return !empty();
	}

	// get pointer

	Abstract* get() {
		return detail::cast_as_one_of<Abstract, Variants...>(&data_, active_idx_);
	}

	constexpr
	const Abstract* get() const {
		return detail::cast_as_one_of<Abstract, Variants...>(&data_, active_idx_);
	}

	Abstract* operator -> () {
		return get();
	}

	constexpr
	const Abstract* operator -> () const {
		return get();
	}

	// get reference

	Abstract& operator * () {
		return *get();
	}

	constexpr
	const Abstract& operator * () const {
		return *get();
	}

	// modify

	void erase() {
		if (empty())
            return;
        detail::destruct_as_one_of<void, Variants...>(reinterpret_cast<Abstract*>(&data_), active_idx_);
        active_idx_ = sizeof...(Variants);
	}

	template<typename T, typename ...Args>
	void emplace(Args&&... args) {
		static_assert(!all_false<std::is_same<T, Variants>...>::value,
				"Der Templateparamter ist keiner der speicherbaren Typen. "
				"Typ muss zu VariantClass-Definition als Parameter hinzugefügt werden.");
		erase();
		construct(reinterpret_cast<T*>(&data_), std::forward<Args>(args)...);
        active_idx_ = detail::index_of_type<T, Variants...>();
        assert(active_idx_ < sizeof... (Variants));
	}

private:
	UnionStorage<Variants...> data_;
    std::size_t active_idx_;
};

} // namespace TURAG

#endif // TINAPP_CONTAINER_VARIANT_CLASS_H
