/**
 *  @brief		Base class for all TURAG feldbus devices
 *  @file		feldbus_basedevice.h
 *  @date		28.09.2022
 *  @author		Martin Oemus
 *
 */


#ifndef TURAGFELDBUSBASEDEVICE_H_
#define TURAGFELDBUSBASEDEVICE_H_

#include <tina++/tina.h>
#include <tina/feldbus/protocol/turag_feldbus_bus_protokoll.h>
#include <tina++/feldbus/host/feldbusabstraction.h>


#if !TURAG_USE_TURAG_FELDBUS_HOST
# warning TURAG_USE_TURAG_FELDBUS_HOST must be defined to 1
#endif


/// Definiert, wie oft eine Übertragung wiederholt wird, bis mit einem Fehler
/// abgebrochen wird, falls im Konstruktor kein anderer Wert übergeben wird.
#if !defined(TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ATTEMPTS) || defined(__DOXYGEN__)
# define TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ATTEMPTS		5
#endif

/// Checksummen-Algorithmus der beim Instanziieren von Feldbusklassen
/// standardmäßig benutzt wird.
#if !defined(TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE) || defined(__DOXYGEN__)
# define TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE			TURAG::Feldbus::ChecksumType::crc8_icode
#endif


/*!
 * @}
 */



namespace TURAG {
namespace Feldbus {


/**
 *
 */
class BaseDevice {
    NOT_COPYABLE(BaseDevice);
public:
    /*!
     * \brief Struktur-Template zur Verwendung mit transceive().
     *
     * address und checksum werden dann automatisch befüllt.
     */
    template<typename T = void>
    struct Broadcast {
        uint8_t address;
        uint8_t id;
        T data;
        uint8_t checksum;
    } _packed;

    /**
     * \brief Struktur-Template zur Verwendung mit transceive().
     *
     * address und checksum werden dann automatisch befüllt.
     */
    template<typename T = void>
    struct Request {
        uint8_t address;
        T data;
        uint8_t checksum;
    } _packed;

    /**
     * \brief Struktur-Template zur Verwendung mit transceive().
     *
     * address und checksum werden dann automatisch befüllt.
     */
    template<typename T = void>
    struct Response {
        uint8_t address;
        T data;
        uint8_t checksum;
    } _packed;



    /**
     * \brief Konstruktor.
     * \param[in] feldbus Referenz auf eine FeldbusAbstraction-Instanz, an die die
     * eigentliche Datenübertragung delegiert wird.
     * \param[in] type Typ der verwendeten Checksumme.
     * \param[in] max_transmission_attempts Maximale Anzahl von Versuchen, ein Paket zu übertragen.
     */
    BaseDevice(FeldbusAbstraction& feldbus,
           ChecksumType type = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE,
           unsigned int max_transmission_attempts = TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ATTEMPTS) :
        myTotalTransmissions(0),
        myTotalChecksumErrors(0),
        myTotalNoAnswerErrors(0),
        myTotalMissingDataErrors(0),
        myTotalTransmitErrors(0),
        bus_(feldbus), maxTransmissionAttempts(max_transmission_attempts), myChecksumType(type)
    {
    }

#if TURAG_USE_LIBSUPCPP_RUNTIME_SUPPORT
    virtual ~BaseDevice() { }
#endif


    /**
     * @brief Zugriff auf den mit dem Gerät assoziierten Bus.
     * @return Referenz auf eine FeldbusAbstraction-Instanz.
     */
    FeldbusAbstraction& bus(void) const { return bus_; }

protected:
    /*!
     * \brief Sendet Daten zum Slave und empfängt eine Antwort.
     * \param[in] target address
     * \param[in] transmit Puffer der zu übertragenden Daten.
     * \param[in] transmit_length Länge des Puffers.
     * \param[out] receive Puffer für die Empfangsdaten.
     * \param[in] receive_length Länge des Empfangspuffers/der erwarteten Daten.
     * ist.
     * \return True bei erfolgreicher Übertragung.
     *
     * Diese Funktion kann benutzt werden, um High-Level-Zugriffsfunktionen in
     * abgeleiteten Klassen bereitzustellen. Wenn möglich sollte allerdings
     * das transceive-Funktions-Template in Verbindung mit Request und Response bzw.
     * Broadcast bevorzugt werden.
     *
     * \warning Diese Funktion überschreibt den Anfang des Puffers mit der
     * Adresse (je nach Adresslänge) und den Ende des Puffers mit der Checksumme.
     * Die Nutzdaten müssen also dazwischen platziert werden.
     *
     */
    FeldbusAbstraction::ResultStatus transceive(uint8_t address, uint8_t *transmit, int transmit_length, uint8_t *receive, int receive_length, unsigned maxAttempts = 0);

    /*!
     * \brief Blockierende Standard-Datenüberträgung.
     * \param[in] transmit Zu sendende Daten, verpackt in einer Request-Struktur.
     * \param[out] receive Pointer auf eine Response-Struktur, die nach dem Aufruf die
     * Antwortdaten enthält.
     * \param[in] ignoreDysfunctional Sendet das Paket auch, wenn das Gerät dysfunktional
     * ist.
     * \return True bei erfolgreicher Übertragung.
     *
     * Sendet ein Datenpaket an das Slavegerät und blockiert solange, bis entweder
     * die erwartete Anzahl an Daten eingetroffen ist oder ein timeout erreicht wurde.
     *
     * Diese Funktion benutzt intern die plattformabhängige Funktion turag_rs485_transceive(),
     * deren Implementierung bestimmt, wie die Übertragung im Detail stattfindet.
     */
    template<typename T, typename U> _always_inline
    FeldbusAbstraction::ResultStatus transceive(uint8_t address, Request<T>& transmit, Response<U>* receive) {
        return transceive(
                    address,
                    reinterpret_cast<uint8_t*>(&(transmit)), sizeof(Request<T>),
                    reinterpret_cast<uint8_t*>(receive), sizeof(Response<U>));
    }

    /*!
     * \brief Versendet einen Broadcast mit Antwort.
     * \param[in] transmit Zu sendende Daten, verpackt in einer Broadcast-Struktur.
     * \param[out] receive Pointer auf eine Response-Struktur, die nach dem Aufruf die
     * Antwortdaten enthält.
     * \param[in] ignoreDysfunctional Sendet das Paket auch, wenn das Gerät dysfunktional
     * ist.
     * \return True bei erfolgreicher Übertragung.
     *
     * Sendet ein Datenpaket an das Slavegerät und blockiert solange, bis entweder
     * die erwartete Anzahl an Daten eingetroffen ist oder ein timeout erreicht wurde.
     *
     * Diese Funktion benutzt intern die plattformabhängige Funktion turag_rs485_transceive(),
     * deren Implementierung bestimmt, wie die Übertragung im Detail stattfindet.
     */
    template<typename T, typename U> _always_inline
    FeldbusAbstraction::ResultStatus transceive(uint8_t address, Broadcast<T>& transmit, Response<U>* receive) {
        return transceive(
                    address,
                    reinterpret_cast<uint8_t*>(&(transmit)), sizeof(Broadcast<T>),
                    reinterpret_cast<uint8_t*>(receive), sizeof(Response<U>));
    }

    /*!
     * \brief Versendet einen Broadcast.
     * \param[out] transmit Zu sendende Daten, verpackt in eine Broadcast-Struktur.
     * \param[in] ignoreDysfunctional Sendet das Paket auch, wenn das Gerät dysfunktional
     * ist.
     * \return True bei erfolgreicher Übertragung.
     *
     * Sendet einen %Broadcast an die in transmit eingestellte Gerätegruppe und
     * kehrt zurück, sobald alle Daten geschrieben wurden.
     */
    template<typename T> _always_inline
    FeldbusAbstraction::ResultStatus transceive(uint8_t address, Broadcast<T>& transmit) {
        return transceive(
                    address,
                    reinterpret_cast<uint8_t*>(&(transmit)), sizeof(Broadcast<T>),
                    nullptr, 0);
    }



protected:
    unsigned int myTotalTransmissions;
    unsigned int myTotalChecksumErrors;
    unsigned int myTotalNoAnswerErrors;
    unsigned int myTotalMissingDataErrors;
    unsigned int myTotalTransmitErrors;


private:
    FeldbusAbstraction& bus_;

    const unsigned int maxTransmissionAttempts;
    ChecksumType myChecksumType;
};



// specializations of the container templates

/**
* \brief Struktur-Template zur Verwendung mit transceive().
*
* Für Übertragungen, die nur aus Adresse, Broadcast-ID und Checksumme bestehen.
*/
template<>
struct BaseDevice::Broadcast<void> {
    uint8_t address;
    uint8_t id;
    uint8_t checksum;
} _packed;

/**
* \brief Struktur-Template zur Verwendung mit transceive().
*
* Für Übertragungen, die nur aus Adresse und Checksumme bestehen.
*/
template<>
struct BaseDevice::Request<void> {
    uint8_t address;
    uint8_t checksum;
} _packed;

/**
* \brief Struktur-Template zur Verwendung mit transceive().
*
* Für Übertragungen, die nur aus Adresse und Checksumme bestehen.
*/
template<>
struct BaseDevice::Response<void> {
    uint8_t address;
    uint8_t checksum;
} _packed;



} // namespace Feldbus
} // namespace TURAG

#endif /* TURAGFELDBUSBASEDEVICE_H_ */
