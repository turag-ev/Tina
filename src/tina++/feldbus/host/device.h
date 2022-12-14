/**
 *  @brief		Base class for all TURAG feldbus devices
 *  @file		device.h
 *  @date		22.03.2013
 *  @author		Martin Oemus
 *
 */


#ifndef TINAPP_FELDBUS_HOST_DEVICE_H
#define TINAPP_FELDBUS_HOST_DEVICE_H

#include <tina++/debug/errorobserver.h>
#include <tina++/feldbus/host/feldbusabstraction.h>
#include <tina++/thread.h>
#include <tina++/time.h>
#include <tina++/tina.h>
#include <tina/feldbus/protocol/turag_feldbus_bus_protokoll.h>
#include "feldbus_basedevice.h"


#if !TURAG_USE_TURAG_FELDBUS_HOST
#warning TURAG_USE_TURAG_FELDBUS_HOST must be defined to 1
#endif


/**
 * @defgroup feldbus-host Host Implementierungen [C++]
 * @ingroup feldbus
 * 
 * Host-Klassen, die zur Kommunikation mit Busgeräten benutzt werden können.
 * 
 * Die Basis aller Hosts-Klassen ist \ref TURAG::Feldbus::Device, das masterseitigen 
 * Support für das Basis-Protokoll bereitstellt.
 * 
 * Das Verhalten des Masters kann mit einigen Definitionen angepasst werden, für die
 * sinnvolle Standardwerte eingestellt sind. Bei Bedarf können diese über die TinA-Konfiguration
 * entsprechend überschrieben werden.
 *
 * Die Kommunikation mit der Hardware-Schnittstelle wird mit der TURAG::Feldbus::FeldbusAbstraction-Klasse
 * abstrahiert und muss mit einer Subklasse um den plattformspezifischen Teil
 * erweitert werden.
 *
 * \section feldbus-host-configurability Konfigurierbarkeit
 * - \ref TURAG_USE_TURAG_FELDBUS_HOST muss auf 1 definiert sein.
 * - \ref TURAG_FELDBUS_AKTOR_STRUCTURED_OUTPUT_AVAILABLE ist optional.
 *
 * 
 * \section feldbus-host-threadsafety Thread-Safety
 * \note Generell ist aus Gründen der Einfachheit zu empfehlen, alle Host-Klassen
 * exklusiv von einem einzigen %Thread aus zu benutzen und den Austausch von Daten
 * über andere Mechanismen zu gewährleisten. Dadurch ist der Zugriff auf den 
 * Bus geordnet und die Komplexität des System wird nicht unnnötig gesteigert.
 * Die folgenden Informationen sind der Vollständigkeit halber angegeben.
 * 
 * Damit der Buszugriff mit mehreren Threads reibungslos funktioniert, sind
 * folgende Voraussetzungen zu erfüllen:
 * - Host-Klassen müssen mindestens reentrant sein
 * - Zugriff auf die Bus-Hardware muss synchronisert sein
 * - Bus-Transaktionen müssen atomaren Charakter besitzen.
 * 
 * Die Host-Klassen sind reentrant, aber nicht thread-safe. Das heißt, der Zugriff 
 * auf eine Instanz einer Klasse muss manuell synchronisiert werden. Unproblematisch 
 * ist hingegen die Verwendung verschiedener Instanzen einer Klasse in verschiedenen Threads.
 * Dies ist eine übliche Einschränkung, da die Gewährleistung von Threadsicherheit
 * für jede Funktion einen zu großen Overhead mit sich bringen würde. Soll ein Busgerät
 * von mehreren Threads angesprochen werden, so müssen entweder zwei Instanzen 
 * für jeden %Thread angelegt werden oder jeder Aufruf von nicht-const Funktionen muss
 * synchronisert werden. Welche Variante sinnvoller ist, hängt stark von den Details
 * der entsprechenden Klasse ab.
 * 
 * Der Zugriff auf den Bus wird durch die Klasse TURAG::Feldbus::FeldbusAbstraction gewährleistet.
 * Im Konstruktor kann angegeben werden, ob eine Semaphore zur Synchronisierung benutzt 
 * werde soll.
 * 
 * Atomizität der Bus-Transaktionen bedeutet, dass falls für eine bestimmte
 * Funktionalität in einem Gerät mehr als eine Bus-Transaktion (Paket senden und
 * empfangen) erforderlich ist, die sequentielle Durchführung aller notwendigen Transaktionen
 * nicht durch Fremdpakete unterbrochen wird. Da jedoch das
 * Basis-Protokoll zustandslos ist und dies auch von allen Geräte-Protokollen
 * verlangt wird, kann eine solche Konstellation theoretisch nicht auftreten und 
 * es müssen dahingehend keine Maßnahmen getroffen werden.
 * 
 *
 * \addtogroup feldbus-host
 * @{
 */

/// Definiert, wieviele Übetragungen hintereinander fehlschlagen müssen,
/// damit das Gerät als dysfunktional deklariert wird, falls im Konstruktor
/// kein anderer Wert angegeben wird.
#if !defined(TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ERRORS) || defined(__DOXYGEN__)
# define TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ERRORS			7
#endif


/*!
 * @}
 */


namespace TURAG {

/**
 * @brief TURAG-Feldbus Klassen.
 *
 * Alle Host-Geräte leiten sich von Device ab, welches zusammen mit
 * FeldbusAbstraction das Basis-Protokoll auf Hostseite implementiert.
 * 
 * Slave-Implementierungen sind im Namspace \ref Feldbus::Slave zu finden.
 *
 * @ingroup feldbus-host
 */
namespace Feldbus {


/**
 * \brief Basis-Klasse aller Feldbus-Geräte.
 * 
 * Die Device-Klasse implementiert auf Hostseite das Basis-Protkoll
 * des %TURAG-Feldbus (zusammen mit FeldbusAbstraction).
 * 
 * Zu Beginn der Initialisierung jedes Gerätes sollte isAvailable() aufgerufen werden,
 * damit nicht funktionierende Geräte nicht erst zum Zeitpunkt ihres ersten Einsatzes
 * identifiziert werden, da das u.U. etwas dauern kann.
 * 
 * Während des Betriebs kann isAvailable() jederzeit benutzt werden um den Status
 * des Gerätes zu prüfen, da dieser Aufruf nur einmalig Buslast verursacht.
 * 
 * Wenn für ein Gerät zu viele Übertragungsfehler in Folge registriert werden,
 * so wird dieses auf unbestimmte Zeit als "dysfunktional" deklariert. Sämtliche
 * an dieses Gerät gerichtete Pakete werden verworfen. Besteht Grund zur Annahme,
 * dass der Fehler im Gerät temporärer Natur war (z.B. Spannungseinbruch und infolgedessen
 * Reset), so sollte in gewissen Abständen sendPing() benutzt werden, um die
 * erneute Erreichbarkeit des Gerätes zu prüfen. Gelingt eine Übertragung, so
 * wird das Gerät nicht länger als dysfunktional betrachtet und auch alle weiteren Pakete
 * werden erneut transportiert.
 * 
 * Instanzen dieser Klasse können je nach Bedarf mit verschiedenen Adresslängen 
 * gebildet werden.
 * 
 */
class Device : public BaseDevice {
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

public:
    /*!
     * \brief Speichert das Device-Info-Paket eines Slave-Gerätes.
     *
     * Diese Struktur entspricht vom Speicherlayout nicht dem, was der
     * Slave über den Bus sendet.
     */
    class DeviceInfo {
        friend class Device;

    public:
        DeviceInfo()
            : deviceProtocolId_(0),
              deviceTypeId_(0),
              crcDataField_(0xFF),
              extendedDeviceInfoPacketSize_(0),
              uuid_(0),
              uptimeFrequency_(0) {}

        bool isValid(void) { return crcDataField_ != 0xFF; }

        uint8_t deviceProtocolId(void) const { return deviceProtocolId_; }
        uint8_t deviceTypeId(void) const { return deviceTypeId_; }
        ChecksumType crcType(void) const { return static_cast<ChecksumType>(crcDataField_ & 0x07); }
        bool packageStatisticsAvailable(void) const { return crcDataField_ & 0x80; }
        bool legacyDeviceInfoPacket() const { return !(crcDataField_ & 0x08); }
        uint16_t uptimeFrequency(void) const { return uptimeFrequency_; }
        uint16_t extendedDeviceInfoPacketSize(void) const { return extendedDeviceInfoPacketSize_; }
        uint32_t uuid(void) const { return uuid_; }

    private:
        uint8_t deviceProtocolId_;
        uint8_t deviceTypeId_;
        uint8_t crcDataField_;
        uint16_t extendedDeviceInfoPacketSize_;
        uint32_t uuid_;
        uint16_t uptimeFrequency_;
    };

    class ExtendedDeviceInfo {
        friend class Device;

    public:
        ExtendedDeviceInfo() : nameLength_(0), versioninfoLength_(0), bufferSize_(0) {}

        uint8_t nameLength(void) const { return nameLength_; }
        uint8_t versionInfoLength(void) const { return versioninfoLength_; }
        uint16_t bufferSize(void) const { return bufferSize_; }
        bool isValid() const { return bufferSize_ > 0; }

    private:
        uint8_t nameLength_;
        uint8_t versioninfoLength_;
        uint16_t bufferSize_;
    };


    /**
	 * \brief Konstruktor.
	 * \param[in] name_ Interner Name des Gerätes, der von name() zurückgeliefert wird.
	 * \param[in] address Busadresse des Gerätes.
     * \param[in] feldbus Referenz auf eine FeldbusAbstraction-Instanz, an die die
     * eigentliche Datenübertragung delegiert wird.
	 * \param[in] type Typ der verwendeten Checksumme.
	 * \param[in] max_transmission_attempts Maximale Anzahl von Versuchen, ein Paket zu übertragen.
	 * \param[in] max_transmission_errors Anzahl hintereinander auftretender Fehler, ab dem das Gerät als dysfunktional deklariert wird.
	 * \param[in] addressLength Auf dem Bus verwendete Adresslänge.
	 */
    Device(const char* name_, unsigned address, FeldbusAbstraction& feldbus,
           ChecksumType type = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE,
           unsigned int max_transmission_attempts = TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ATTEMPTS,
           unsigned int max_transmission_errors = TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ERRORS);

#if TURAG_USE_LIBSUPCPP_RUNTIME_SUPPORT
    virtual ~Device() {}
#endif

    /**
	 * \brief Gibt zurück, ob das Gerät verfügbar ist.
	 * \param[in] forceUpdate Gibt an, ob in jedem Fall ein 
	 * Ping-Paket versendet werden soll, um eine möglichst aktuelle
	 * Information zu erhalten. Ist das Gerät dysfunktional, wird allerdings
	 * trotzdem nicht der Versuch unternommen, ein Paket zu senden.
	 * \return True wenn das Gerät verfügbar ist, ansonsten false.
	 * 
	 * Diese Funktion informiert über den Verfügbarkeitsstatus des
	 * Gerätes. Dazu versucht sie bei ihrem ersten Aufruf solange
	 * ein Ping-Paket zu senden, bis dies entweder erfolgreich ist
	 * oder die maximale Anzahl an Fehler erreicht wurde. 
	 * 
	 * Im Normalfall gelingt die Übertragung des Ping-Paketes. Von da
	 * an verursacht die Funktion keine Buslast mehr, sondern gibt lediglich
	 * den aktuellen Fehlerstatus des Gerätes zurück. Dadurch ist diese
	 * Funktion bestens geeignet, wenn eine Verfügbarkeitsinformation 
	 * mit einer großen Häufigkeit, doch mittleren Verlässlichkeit benötigt wird.
	 * 
	 * Ist eine
	 * besonders aktuelle Information nötig, so kann das Verhalten des ersten
	 * Aufrufs mit dem forceUpdate-Parameter erzwungen werden - in diesem Fall
	 * werden solange Pakete gesendet, bis 
	 * eine Übertragung erfolgreich verläuft oder das Gerät als dysfunktional 
	 * deklariert wird.
	 * 
	 * Gibt die Funktion einmal false zurück, so wird sich daran von selbst
	 * nichts mehr ändern, weil zu diesem Zeitpunkt sämtliche Pakete verworfen
	 * werden (auch dann, wenn forceUpdate benutzt wird!). Gibt diese Funktion false zurück
	 * obwohl das Gerät funktionieren sollte, so kann entweder mit clearTransmissionCounters()
	 * der interne Fehler-Counter zurückgesetzt oder versucht werden, mit sendPing()
	 * das Gerät zu erreichen.
	 */
    bool isAvailable(bool forceUpdate = false);

    /**
	 * @brief Gibt den Namen des Gerätes zurück.
	 * @return Name des Gerätes.
	 *
	 * Hierbei handelt es sich um den Master-seitig vergebenen Gerätenamen, der
	 * sich durchaus von der Bezeichnung unterscheiden kann, die ein Slave über
	 * sich selbst zurück gibt.
	 *
	 * \see receiveDeviceRealName
	 */
    const char* name(void) const { return name_; }

    /*!
	 * \brief Gibt die Adresse des Gerätes zurück.
	 * \return Adresse des Gerätes.
	 */
    unsigned address(void) const { return myAddress; }

    /*!
	 * \brief Gibt die Geräte-Info zurück.
	 * \param[out] device_info Pointer auf DeviceInfo, was nach dem Aufruf die Daten
	 * enthält.
	 * \return True bei erfolgreicher Übertragung, ansonsten false.
	 * 
	 * Da sich die Geräteinformation nicht während des Betriebs ändern
	 * können, werden diese beim ersten erfolgreichen Aufruf von getDeviceInfo()
	 * gepuffert, wodurch folgende Aufrufe keine Buslast mehr verursachen.
	 * 
	 * Wird die Device-Info von innerhalb dieser oder einer Kindklasse benötigt,
	 * so kann diese Funktion mit null als Argument aufgerufen werden.
	 * Die Daten werden dann in myDeviceInfo gepuffert (falls dies nicht bereits
	 * zu einem früheren Zeitpunkt geschehen ist) und können von dort
	 * ausgelesen werden.
	 */
    bool getDeviceInfo(DeviceInfo* device_info);

    bool getExtendedDeviceInfo(ExtendedDeviceInfo* extended_device_info);

    /*!
	 * \brief Empfängt die im Gerät hinterlegte Bezeichnung des Slaves.
	 * \param[out] out_real_name Char-Array, das den String aufnimmt.
	 * \return True bei erfolgreichem Empfang der Gerätebezeichnung.
	 * 
	 * Diese Funktion puffert den empfangenen String nicht und verursacht 
	 * daher bei jedem Aufruf erneute Buslast.
	 * 
	 * \warning Die DeviceInfo-Struktur gibt die Länge des Gerätenamens 
	 * ohne Nullzeichen an, während diese Funktion einen null-terminierten
	 * String zurückgibt. Daher muss out_real_name ein Byte größer reserviert
	 * werden!
	 */
    bool receiveDeviceRealName(char* out_real_name);

    /*!
	 * \brief Empfängt die Versionsinfo des Gerätes.
	 * \param[out] out_version_info Char-Array, das den String aufnimmt.
	 * \return True bei erfolgreichem Empfang der Versionsinfo.
	 * 
	 * Diese Funktion puffert den empfangenen String nicht und verursacht 
	 * daher bei jedem Aufruf erneute Buslast.
	 * 
	 * \warning Die DeviceInfo-Struktur gibt die Länge der Versionsinfo 
	 * ohne Nullzeichen an, während diese Funktion einen null-terminierten
	 * String zurückgibt. Daher muss out_version_info ein Byte größer reserviert
	 * werden!
	 */
    bool receiveVersionInfo(char* out_version_info);

    /**
	 * \brief Fragt die Uptime des Gerätes ab.
	 * \param[out] uptime Puffer in dem die Uptime gespeichert wird.
	 * \return True bei Erfolg, ansonsten false.
	 * 
	 * Angabe in Sekunden.
	 * 
	 * Wenn das Gerät die eigene Uptime nicht zählt, so wird 
	 * NaN in uptime geschrieben.
	 */
    bool receiveUptime(float* uptime);

    /**
	 * \brief Fragt die Anzahl korrekt empfangener Pakete vom Slave ab.
	 * \param[out] packageCount Puffer in dem der Wert gespeichert wird.
	 * \return True bei Erfolg, ansonsten false.
	 * \see https://intern.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;turag-simplebus/#fehlerzustaende
	 */
    bool receiveNumberOfAcceptedPackages(uint32_t* packageCount);

    /**
	 * \brief Fragt die Anzahl aufgetretener Pufferüberlaufe vom Slave ab.
	 * \param[out] overflowCount Puffer in dem der Wert gespeichert wird.
	 * \return True bei Erfolg, ansonsten false.
	 * \see https://intern.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;turag-simplebus/#fehlerzustaende
	 */
    bool receiveNumberOfOverflows(uint32_t* overflowCount);

    /**
	 * \brief Fragt die Anzahl verlorener Pakete vom Slave ab.
	 * \param[out] lostPackagesCount Puffer in dem der Wert gespeichert wird.
	 * \return True bei Erfolg, ansonsten false.
	 * \see https://intern.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;turag-simplebus/#fehlerzustaende
	 */
    bool receiveNumberOfLostPackages(uint32_t* lostPackagesCount);

    /**
	 * \brief Fragt die Anzahl aufgetretener Checksummenfehler vom Slave ab.
	 * \param[out] checksumErrorCount Puffer in dem der Wert gespeichert wird.
	 * \return True bei Erfolg, ansonsten false.
	 * \see https://intern.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;turag-simplebus/#fehlerzustaende
	 */
    bool receiveNumberOfChecksumErrors(uint32_t* checksumErrorCount);

    /**
	 * \brief Fragt die Anzahl korrekt empfangener Pakete, aufgetretener Pufferüberlaufe, verlorener Pakete und 
	 * aufgetretener Checksummenfehler vom Slave ab.
	 * \param[out] counts Puffer in dem der Wert gespeichert wird.
	 * \return True bei Erfolg, ansonsten false.
	 * \see https://intern.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;turag-simplebus/#fehlerzustaende
	 * 
	 * Es werden mit einem Mal alle 4 Werte abgefragt, der Puffer muss also eine
	 * entsprechende Größe aufweisen.
	 */
    bool receiveAllSlaveErrorCount(uint32_t* counts);

    /**
	 * \brief Weist das Gerät an, seine internen Paketstatistiken zurückzusetzen.
	 * \return True bei Erfolg, anonsten false.
	 */
    bool resetSlaveErrors(void);

    /**
	 * \brief Sendet ein Ping-Paket.
	 * \return True, wenn des Gerät erwartungsgemäß geantwortet hat, false
	 * im Fehlerfall.
	 *
	 * Diese Funktion sendet immer ein Paket, auch wenn das Gerät dysfunktional ist.
	 * Ist die Übertragung erfolgreich, so wird das Gerät wieder als funktionierend
	 * betrachtet, indem der interne Fehler Counter zurückgesetzt wird.
	 * 
	 * \note Diese Funktion sollte verwendet werden, um für dysfunktionale Geräte
	 * zu prüfen, ob diese eventuell wieder verfügbar sind.
	 */
    bool sendPing(void);

    /**
	 * \brief Gibt die Anzahl der im Master aufgetretenen Checksummenfehler zurück.
	 * \return Anzahl der Checksummenfehler.
	 * \see https://intern.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;turag-simplebus/#fehlerzustaende
	 */
    unsigned int getChecksumErrors(void) const { return myTotalChecksumErrors; }

    /**
	 * \brief Gibt die Anzahl der im Master aufgetretenen Antwortfehler zurück.
	 * \return Anzahl der Antwortfehler.
	 * \see https://intern.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;turag-simplebus/#fehlerzustaende
	 */
    unsigned int getNoAnswerErrors(void) const { return myTotalNoAnswerErrors; }

    /**
	 * \brief Gibt die Anzahl der im Master aufgetretenen Datenfehler zurück.
	 * \return Anzahl der Datenfehler.
	 * \see https://intern.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;turag-simplebus/#fehlerzustaende
	 */
    unsigned int getMissingDataErrors(void) const { return myTotalMissingDataErrors; }

    /**
	 * \brief Gibt die Anzahl der im Master aufgetretenen Sendefehler zurück.
	 * \return Anzahl der Sendefehler.
	 * \see https://intern.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;turag-simplebus/#fehlerzustaende
	 */
    unsigned int getTransmitErrors(void) const { return myTotalTransmitErrors; }

    /**
	 * \brief Gibt die Anzahl aller aufgetretenen Fehler zurück.
	 * \return Gesamtzahl der Fehler.
	 */
    unsigned int getAllErrors(void) const { return myTotalChecksumErrors + myTotalNoAnswerErrors + myTotalMissingDataErrors + myTotalTransmitErrors; }

    /**
	 * \brief Gibt die Anzahl der durchgeführten Datenübertragungen zurück.
	 * \return Anzahl der durchgeführten Datenübertragungen.
	 * 
	 * Dieser Wert repräsentiert die Gesamtanzahl an Übertragungsversuchen, schließt also 
	 * die Fehlerfälle mit ein.
	 */
    unsigned int getTotalTransmissions(void) const { return myTotalTransmissions; }

    /**
	 * \brief Setzt die Error-Counter und den Übertragungs-Counter auf null zurück.
	 * 
	 * Nach Aufruf dieser Funktion wird ein Gerät nicht mehr als dysfunktional betrachtet
	 * und Übertragungen werden erneut versucht.
	 * 
	 * \note Falls nicht klar ist, ob ein Gerät noch dysfunktional ist, sollte
	 * statt dieser Funktion sendPing() benutzt werden. Ein Aufruf dieser Funktion
	 * ist dann nur nötig um die bisherigen Übertragungsstatistiken zurückzusetzen.
	 */
    void clearTransmissionCounters(void) {
        myTotalChecksumErrors = 0;
        myTotalNoAnswerErrors = 0;
        myTotalMissingDataErrors = 0;
        myTotalTransmitErrors = 0;
        myTotalTransmissions = 0;
        myCurrentErrorCounter = 0;
        hasCheckedAvailabilityYet = false;
        dysFunctionalLog_.resetAll();
    }


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
    template <typename T, typename U>
    TURAG_ALWAYS_INLINE bool transceive(Request<T>& transmit, Response<U>* receive, bool ignoreDysfunctional = false) {
        return transceive(
                    reinterpret_cast<uint8_t*>(&(transmit)), sizeof(Request<T>),
                    reinterpret_cast<uint8_t*>(receive), sizeof(Response<U>),
                    ignoreDysfunctional);
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
    template<typename T, typename U> TURAG_INLINE
    bool transceive(Broadcast<T>& transmit, Response<U>* receive, bool ignoreDysfunctional = false) {
        return transceive(
                    reinterpret_cast<uint8_t*>(&(transmit)), sizeof(Broadcast<T>),
                    reinterpret_cast<uint8_t*>(receive), sizeof(Response<U>),
                    ignoreDysfunctional, true);
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
    template <typename T>
    TURAG_ALWAYS_INLINE bool transceive(Broadcast<T>& transmit, bool ignoreDysfunctional = false) {
        return transceive(
                    reinterpret_cast<uint8_t*>(&(transmit)), sizeof(Broadcast<T>),
                    nullptr, 0,
                    ignoreDysfunctional, true);
    }

    /**
	 * @brief Gibt das erste Feldbus-Device zurück.
	 * @return Zeiger auf das erste Feldbus-Device im System.
	 *
	 * Über alle Instanzen von Device wird eine List geführt,
	 * über die zu Informationszwecken iteriert werden kann.
	 *
	 * \see getNextDevice()
	 */
    static Device* getFirstDevice(void) {
        return firstDevice;
    }

    /**
	 * @brief Gibt das nächste Feldbus-Device zurück.
	 * @return Zeiger auf das nächste Feldbus-Device im System.
	 *
	 * \see getFirstDevice()
	 */
    Device* getNextDevice(void) const {
        return myNextDevice;
    }


protected:
    /*!
     * \brief Sendet Daten zum Slave und empfängt eine Antwort.
     * \param[in] transmit Puffer der zu übertragenden Daten.
     * \param[in] transmit_length Länge des Puffers.
     * \param[out] receive Puffer für die Empfangsdaten.
     * \param[in] receive_length Länge des Empfangspuffers/der erwarteten Daten.
	 * \param[in] ignoreDysfunctional Sendet das Paket auch, wenn das Gerät dysfunktional
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
	 * Wenn receive oder receive_length 0 ist, so wird angenommen dass eine 
	 * Broadcastübertragung gewünscht wird und statt der Geräteadresse 0 benutzt.
     */
    bool transceive(uint8_t *transmit, int transmit_length, uint8_t *receive, int receive_length, bool ignoreDysfunctional = false, bool transmitBroadcast = false);

    /**
	 * \brief Gibt zurück, ob das Gerät als dysfunktional betrachtet wird.
	 * \return Wenn das Gerät dysfunktional ist true, ansonsten false.
	 * 
	 * Ein Gerät wird als dysfunktional eingestuft, wenn hintereinander zu viele 
	 * Übertragungsfehler registriert werden. Von diesem Zeitpunkt an werden alle
	 * Datenübertragungen abgebrochen.
	 * 
	 * Diese Funktion prüft lediglich ob
	 * \code
	 * myCurrentErrorCounter >= maxTransmissionErrors
	 * \endcode
	 * und wird von der isAvailable()-Funktion verwendet, welche
	 * statt dieser benutzt werden sollte, da diese mehr High-Level-Logik
	 * implementiert (und in den meisten Fällen das ist, was man eigentlich
	 * braucht).
	 */
    bool isDysfunctional(void) const { return myCurrentErrorCounter >= maxTransmissionErrors; }


private:
    bool receiveString(uint8_t command, uint8_t stringLength, char* out_string);
    bool receiveErrorCount(uint8_t command, uint32_t* buffer);


    // data is ordered to prevent the insertion
    // of padding bytes!
public:
protected:
    static constexpr int myAddressLength = 1;

    /**
	 * \brief Puffert die DeviceInfo-Struktur des Gerätes.
	 */
    DeviceInfo myDeviceInfo;
    ExtendedDeviceInfo myExtendedDeviceInfo;


private:
    Debug::CheapErrorObserver dysFunctionalLog_;

    Device* myNextDevice;
    static Device* firstDevice;
    static Mutex listMutex;

    const char* name_;


    const unsigned int maxTransmissionErrors;

    unsigned int myCurrentErrorCounter;

    const uint8_t myAddress;
    bool hasCheckedAvailabilityYet;
};



} // namespace Feldbus
} // namespace TURAG

#endif // TINAPP_FELDBUS_HOST_DEVICE_H
