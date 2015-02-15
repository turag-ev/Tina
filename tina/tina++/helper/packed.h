#ifndef TINAPP_HELPER_PACKED_H
#define TINAPP_HELPER_PACKED_H

namespace TURAG {

/// \addtogroup Utils
/// \{

////////////////////////////////////////////////////////////////////////////////
// Pointer casts

/// \brief Packt eine beliebige Variable in anderen Typ
///
/// Falls eine Funktion als Parameter in einem bestimmten Typ
/// erwartet, aber den Parameter später wieder an eine Benutzerfunktion
/// übergibt, kann die \ref TURAG::pack Funktion benutzt werden, um die Daten
/// entsprechend in den gefordertern Typ zu "packen".
/// Über die Funktion \ref TURAG::unpack kann die Variable wieder in ihren
/// Ursprungstyp gewandelt werden.
///
/// So kann zum Beispiel von Integer in Zeiger und umgekehrt umgewandelt werden.
/// Die Funktionen kann man nutzen, wenn als Parameter für eine Callback-Funktion
/// ein Zeiger (oder ein anderer Typ) benötigt wird.
/// Man aber viel lieber eine Integer-Variable als Parameter haben möchte.
/// In den Zeiger können alle Typen gepackt werden, die die gleiche oder kleinere Speichergröße aufweisen:
/// \code
/// struct struct_t {
///   char a, b;
/// }
/// void callback_int(void* data) {
///   float parameter = unpack<float>(data);
///   // ...
/// }
///
/// void callback_struct(void* data) {
///   struct_t parameter = unpack<struct_t>(data);
///   // ...
/// }
///
/// // Szenario: Aufruf von callback_int und callback_struct
/// //           nur durch diese Funktion möglich.
/// void call_me(void (*cb)(void*), void* parameter) {
///   cb(parameter);
/// }
///
/// // callback_int indirekt mit Parameter 4 aufrufen
/// call_me(callback_int, pack<void*>(4));
///
/// struct_t s;
/// s.a = 42;
/// s.b = 5;
/// // callback_struct indirekt mit Parameter s aufrufen
/// call_me(callback_struct, pack<void*>(s));
/// \endcode
///
/// \pre Bedingung ist das der Ausgangstyp \a Unpacked weniger oder gleich viel
/// Speicherplatz benötigt als der Zieltyp \a Packed: `sizeof(Unpacked) <= sizeof(Packed)`
///
/// \tparam Packed Typ in den Variable gepackt werden soll.
/// \tparam Unpacked Typ von Variable in Grundzustand (muss nicht angegeben werden)
/// \param src Variable, die in anderen Typ gepackt werden soll
/// \sa TURAG::unpack
template<typename Packed, typename Unpacked>
inline Packed pack(Unpacked src) {
  static_assert(sizeof(Unpacked) <= sizeof(Packed), "type cast fatal because of bit loose");

  union { Unpacked src; Packed dest; } helper;
  helper.src = src;
  return helper.dest;
}

/// \brief Entpackt eine beliebige Variable in anderen Typ
///
/// Entpackt eine mit \ref TURAG::pack "gepackte" Variable in ihren ursprünglich
/// Typ.
///
/// \tparam Unpacked Typ von Variable in Grundzustand
/// \tparam Packed Typ in den Variable von der entpackt wird (muss nicht angegeben werden)
/// \param src Variable, die in anderen Typ gepackt werden soll
/// \sa TURAG::unpack, TURAG::reinterpret_reference
template<typename Unpacked, typename Packed>
inline Unpacked unpack(Packed src) {
  union { Unpacked dest; Packed src; } helper;
  helper.src = src;
  return helper.dest;
}

} // namespace TURAG

#endif // TINAPP_HELPER_PACKED_H
