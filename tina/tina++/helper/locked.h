#ifndef TINA_CONCURRENT_LOCKED_H
#define TINA_CONCURRENT_LOCKED_H

#include "scoped_lock.h"
#include <tina++/thread.h>

namespace TURAG {

///
/// \brief Mutex locken, während ein Objekt angeschaut wird.
///
/// Wird ein Objekt von Benutzercode angeschaut, aber der Zugriff auf das
/// Objekt muss durch ein Mutex geschützt werden, kann diese Klasse verwendet werden:
/// \code
/// // API: Zugriff auf interne Datenstruktur Array bieten:
/// typedef int BigArray[1024][1024];
/// LockedRef<const BigArray> getBigArray() {
///   // Zugriff auf bigarray muss mit mutex geschützt sein.
///   return { bigarray, Mutex::Lock(mutex) };
/// }
///
/// // Benutzercode
/// void func() {
///   // ...
///
///   {
///     auto bigarray = getBigArray();
///     bigarray->... // Zugriff auf bigarray
///   } // Lock für bigarray wird freigegeben
///
///   // ...
/// }
/// \endcode
///
/// \tparam Data Typ von Objekt, was für Verfügung gestellt werden soll.
/// \tparam Mtx Mutex Typ (standardmäßig \ref TURAG::Mutex)
template<typename Data>
class LockedRef {
public:
	/// Typ von Objekt, was für Verfügung gestellt werden soll.
	using DataType = Data;

	/// gelocketes Objekt erstellen
	explicit LockedRef(DataType& ref, Mutex::Lock&& lock) :
		lock_(std::move(lock)), ref_(ref)
	{ }

	/// gelocketes Objekt verschieben
	LockedRef(LockedRef&& ref) :
		lock_(std::move(ref.lock_)), ref_(ref.ref_)
	{ }

	/// Zugriff zu Objekt
	DataType& operator->() {
		return ref_;
	}

	/// Zugriff zu Objekt
	DataType& operator*() {
		return ref_;
	}

private:
	/// gelocker Mutex
	Mutex::Lock lock_;

	/// Referenz zu Objekt
	DataType& ref_;
};

} // namespace TURAG

#endif // TINA_CONCURRENT_LOCKED_H
