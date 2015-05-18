#ifndef TINAPP_DEBUG_GRAPH_H
#define TINAPP_DEBUG_GRAPH_H

#include <tina/debug/defines.h>
#include <tina++/math.h>

#include <initializer_list>
#include <atomic>


namespace TURAG {
namespace Debug {


/**
 * \defgroup Debuggraph Messdatenausgabe
 * \ingroup Debug
 * \brief Ermöglicht die grafische 2D-Darstellung von Daten.
 *
 * Die %Graph-Klassen implementieren das
 * <a href="https://www.turag.de/wiki/doku.php/id,04_programmierung;protokolle_busse;tina-debugprotokoll">TinA-Debugprotokoll.</a>
 *
 * Zum Ausgeben von Daten muss die init()-Funktion einer Instanz einer der Klassen aufgerufen werden, wodurch jedes
 * Diagramm einen Titel erhält. Jedes Diagramm kann eine beliebige Anzahl von Channels enthalten,
 * die ihrerseits in Channelgruppen eingeteilt werden können.
 *
 * Zur Anzeige von Daten können die Funktionen Graph::plot() und Graph::plot2D() benutzt werden.
 * Erstere sollte benutzt werden, wenn alle Datenpunkte die gleiche Zeit (x-Wert) verfügen, da dann weniger
 * Daten übertragen werden müssen.
 *
 * Für jeden %Graph kann einzeln die Ausgabe der Daten mit Graph::enable() und Graph::disable()
 * eingestellt werden. Soll global die Ausgabe unterbunden werden, kann dafür Graph::muteAllGraphOutput()
 * benutzt werden (respektive Graph::releaseAllGraphOutput() zum Aktivieren der Ausgabe). Ob ein %Graph
 * tatsächlich Daten ausgibt, hängt letztendlich von der Und-Verknüpfung der Ausgabeeinstellung der jeweiligen
 * Instanz und der globalen Ausgabeeinstellung ab. Die Funktionen init(), addChannel() und addChannelGroup()
 * werden durch das Aktivieren oder Deaktivieren der Ausgabe nicht beeinflusst. Dies ist notwendig um die
 * Funktion der Grpahenausgabe zu gewährleisten, wenn diese zu anfänglich deaktiviert sind und zu einem
 * späteren Zeitpunkt aktiviert werden. Das hat zur Folge, dass alle Graphen mindestens einmal in der Ausgabe erscheinen,
 * unabhängig vom Aktivierungszustand der Ausgabe.
 *
 * Das Makro \ref TURAG_DEBUG_ENABLE_GRAPH_DEFAULT legt fest, ob die globale Ausgabe standardmäßig aktiviert
 * oder deaktiviert ist.
 *
 * Die Klasse Graph ist ausschließlich zur Verwendung von Subklassen vorgesehen und kann nicht direkt instantiiert werden.
 * Wenn Subklassen eigene init-Funktionen anbieten, so müssen diese auch Graph::init() aufrufen.
 *
 *
 *
 * \note Die Graph-Klassen sind reentrant, aber nicht thread-safe. Im Allgemeinen sollte es aber
 * kein wirklich problematischen Seiteneffekte geben, selbst wenn die gleiche Instanz von
 * mehreren Threads benutzt wird.
 *
 * \{
 */


// output graphs
#if !defined(TURAG_DEBUG_RELEASE_GRAPH_DEFAULT)
# if TURAG_DEBUG_LEVEL > 3 || defined(__DOXYGEN__)
/// \brief Schaltet global die Ausgabe von Graphen frei.
///
/// Wird dieses Symbol auf 1 definiert, entspricht dies einem Aufruf
/// von \ref TURAG::Debug::Graph::releaseAllGraphOutput().
///
/// Kann über Makefile definiert werden.
/// Wird standardmäßig bei \ref TURAG_DEBUG_LEVEL größer 3  auf 1 definiert.
#  define TURAG_DEBUG_ENABLE_GRAPH_DEFAULT 1
# else
#  define TURAG_DEBUG_ENABLE_GRAPH_DEFAULT 0
# endif
#endif


#ifndef __DOXYGEN__

// graph output prefixes
#define TURAG_DEBUG_GRAPH_PREFIX    		"D"
#define TURAG_DEBUG_GRAPH_CREATE    		"n"
#define TURAG_DEBUG_GRAPH_COPY				"r"
#define TURAG_DEBUG_GRAPH_CHANNEL   		"b"
#define TURAG_DEBUG_GRAPH_CHANNEL_FIXED		"B"
#define TURAG_DEBUG_GRAPH_DATA      		"d"
#define TURAG_DEBUG_GRAPH_DATA2D      		"D"
#define TURAG_DEBUG_GRAPH_VERTICAL_MARKER  	"m"
#define TURAG_DEBUG_GRAPH_CHANNELGROUP  	"g"

#endif // __DOXYGEN__



/**
 * @brief Basis-Graph-Klasse.
 */
class Graph {
protected:
	/**
	 * @brief Konstruktor
	 */
	Graph() : index(0), channels(0), name(nullptr), startNewDiagramOnTimeOverflow(false), lastTime(NaNf) {}

public:
	/**
	 * @brief Erzeugt einen Graphen.
	 * @param name Bezeichnung des Graphen.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 *
	 * \note Diese Funktion muss vor Benutzung der Instanz aufgerufen werden.
	 */
	void init(const char* name, bool enabled = true);

	/**
	 * @brief Zeichnet alle zukünftigen Ausgaben in ein neues Diagramm.
	 *
	 * Diese Funktion kann benutzt werden, um verschiedene Messphasen klar
	 * voneinander abzugrenzen.
	 *
	 * Wenn die Ausgabe des Graphen deaktiviert ist, tut diese Funktion nichts.
	 */
	void startNewDiagram(void);

	/**
	 * @brief Gibt zurück, ob das Diagramm bereits initialisiert wurde.
	 */
	bool isInitialized(void) {
		return index != 0;
	}

	/**
	 * @brief Aktiviert die Ausgabe des Graphen.
	 */
	void enable(void) {
		enabled = true;
	}

	/**
	 * @brief Deaktiviert die Ausgabe des Graphen.
	 */
	void disable(void) {
		enabled = false;
	}

	/**
	 * @brief Aktiviert oder deaktiviert die Ausgabe des Graphen.
	 * @param enabled_ Aktivieren oder deaktivieren.
	 */
	void setEnabled(bool enabled_) {
	    enabled = enabled_;
	}

	/**
	 * @brief Gibt zurück, ob die Ausgabe des Graphen aktiviert ist.
	 * @return
	 */
	bool isEnabled(void) {
	    return enabled;
	}

	/**
	 * @brief Toggelt den Ausgabezustand des Graphen und gibt
	 * den neuen Zustand aus.
	 * @return True, wenn die Graphausgabe aktiviert wurde, ansonsten false.
	 */
	bool toggleGraphEnabled(void) {
	    enabled = !enabled;
	    return enabled;
	}

	/**
	 * @brief Schaltet global die Ausgabe von Graphen frei.
	 *
	 * Graphen, die separat mit disable() deaktiviert wurden, werden dennoch nicht
	 * ausgegeben.
	 */
	static void releaseAllGraphOutput(void) {
		allEnabled = true;
	}

	/**
	 * @brief Deaktiviert die Ausgabe alle Graphen.
	 *
	 * Diese Funktion verändert nicht den Ausgabezustand der Graphen an
	 * sich, d.h. deaktivierte Graphen müssen noch einzeln aktiviert werden.
	 */
	static void muteAllGraphOutput(void) {
		allEnabled = false;
	}

	/**
	 * @brief Gibt zurück, ob die Graphenausgabe global deaktiviert ist.
	 * @return True, wenn alle Graphen deaktiviert sind, ansonsten false.
	 */
	static bool isGraphOutputMuted(void) {
	    return !allEnabled;
	}

	/**
	 * @brief Gibt zurück, ob die Graphenausgabe global aktiviert ist.
	 * @return True, wenn alle Graphen aktiviert sind, ansonsten false.
	 */
	static bool isGraphOutputReleased(void) {
	    return allEnabled;
	}

	/**
	 * @brief Fügt jedem Channel einen Datensatz hinzu.
	 * @param time Zeit/x-Achse.
	 * @param data Pointer zu den y-Werten.
	 * @param count Anzahl der Werte.
	 *
	 * count darf maximal so groß sein wie die Anzahl der Channels des Graphen.
	 */
	void plot(float time, float* data, size_t count);

	/**
	 * @brief Fügt jedem Channel einen Datensatz hinzu.
	 * @param time Zeit/x-Achse.
	 * @param data Array mit den y-Werten.
	 *
	 * data darf maximal so viele Elemente haben wie der Graph Channels hat.
	 */
	template <size_t N>
	void plot(float time, float (&data)[N]) {
		plot(time, data, N);
    }

	/**
	 * @brief Fügt jedem Channel einen Datensatz hinzu.
	 * @param channelIndex Index des Channels.
	 * @param x x-Wert
	 * @param y y-Wert
	 *
	 * Mit dieser Funktion kann ein einzelner Datensatz ausgegeben werden.
	 * Channel-Indices beginnen mit 0 und werden in der Reihenfolge der 
	 * Channeldefinition fortlaufend inkrementiert.
	 */
	void plot2D(unsigned channelIndex, float x, float y);

	/**
	 * @brief Erstellt eine Channelgruppe.
	 * @param label Bezeichnung der Gruppe
	 * @param channelIndices Liste mit den Indices der Channels, die Teil der neuen
	 * Gruppe werden sollen.
	 * Channel-Indices beginnen mit 0 und werden in der Reihenfolge der 
	 * Channeldefinition fortlaufend inkrementiert.
	 */
	void addChannelGroup(const char* label, std::initializer_list<unsigned> channelIndices);

	/**
	 * @brief Automatisch neue Diagramme beginnen.
	 * @param on Aktivieren oder deaktivieren.
	 *
	 * Beginnt ein neues Diagramm, wenn plot() mit einer Zeit aufgerufen wird,
     * die kleiner ist als beim letzten Aufruf. Diese Option greift nicht,
     * wenn Daten mit plot2D() ausgegeben werden.
     *
     * Dies passiert zum Beispiel immer dann, wenn das Spiel beginnt und die
     * Zeit mit turag_debug_system_gametime_get() ausgegeben wird.
	 */
    void setStartNewDiagramOnTimeReset(bool on) {
		startNewDiagramOnTimeOverflow = on;
	}

protected:
	unsigned index;
	unsigned channels;

private:
	bool enabled;
	const char* name;
	bool startNewDiagramOnTimeOverflow;
	float lastTime;

	static bool allEnabled;
	static std::atomic<unsigned> graphCount;
};


/**
 * @brief Diagramm mit automatischen zoomender Ansicht.
 *
 * Dieses Diagramm zoomt beim Eintreffen neuer Daten so,
 * dass alle Punkte sichtbar sind.
 */
class GraphAuto : public Graph {
public:
	/**
	 * @brief Erzeugt einen Graphen incl. Channels.
	 * @param name Bezeichnung des Graphen.
	 * @param channelNames Liste mit den Bezeichnungen der Channels.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	void init(const char *name, std::initializer_list<const char*> channelNames, bool enabled = true);

	/**
	 * @brief Erzeugt einen Graphen incl. Channels und Channel-Gruppen.
	 * @param name Bezeichnung des Graphen.
	 * @param channelNames Liste, die Listen mit den Bezeichnungen der Channels enthält,
	 * entsprechend der gewünschten Zuordnung zu den Channel-Gruppen.
	 * @param channelGroups Liste mit den Bezeichnungen der Channel-Gruppen.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	void init(const char *name, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled = true);

	/**
	 * @brief Fügt einen Channel zum Graphen hinzu.
	 * @param title Bezeichnung des Channels.
	 */
	void addChannel(const char* title);
};


/**
 * @brief Diagramm mit fester Ansicht.
 *
 * Dieses Diagramm behält beim Eintreffen neuer Daten die vorgegebene Zoom-Einstellung bei.
 */
class GraphFixedSize : public Graph {
public:
	/**
	 * @brief Erzeugt einen Graphen mit festen Achsen incl. Channels.
	 * @param name Bezeichnung des Graphen.
	 * @param x_left x-Koordinate des Ursprungs.
	 * @param y_bottom y-Koordinate des Ursprungs.
	 * @param width Breite des Koordinatensystems.
	 * @param height Höhe des Koordinatensystems.
	 * @param channelNames Liste mit den Bezeichnungen der Channels.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	void init(const char *name, int x_left, int y_bottom, unsigned width, unsigned height, std::initializer_list<const char*> channelNames, bool enabled = true);

	/**
	 * @brief Erzeugt einen Graphen mit festen Achsen incl. Channels und Channel-Gruppen.
	 * @param name Bezeichnung des Graphen.
	 * @param x_left x-Koordinate des Ursprungs.
	 * @param y_bottom y-Koordinate des Ursprungs.
	 * @param width Breite des Koordinatensystems.
	 * @param height Höhe des Koordinatensystems.
	 * @param channelNames Liste, die Listen mit den Bezeichnungen der Channels enthält,
	 * entsprechend der gewünschten Zuordnung zu den Channel-Gruppen.
	 * @param channelGroups Liste mit den Bezeichnungen der Channel-Gruppen.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	void init(const char *name, int x_left, int y_bottom, unsigned width, unsigned height, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled = true);


	/**
	 * @brief Fügt einen Channel mit festen Achsen zum Graphen hinzu.
	 * @param title Bezeichnung des Channels.
	 * @param x_left x-Koordinate des Ursprungs.
	 * @param y_bottom y-Koordinate des Ursprungs.
	 * @param width Breite des Koordinatensystems.
	 * @param height Höhe des Koordinatensystems.
	 *
	 * Der Channel mit den größten Ausmaßen bestimmt die Größe des Graphen.
	 */
	void addChannel(const char* title, int x_left, int y_bottom, unsigned width, unsigned height);
};


/**
 * @brief Diagramm zur Anzeige eines festen Zeitfensters.
 *
 * Dieses Diagramm verhält sich ähnlich wie die Anzeige eines Oszilloskops:
 * die x-Achse hat eine feste Länge, während sich die y-Achse den ankommenden
 * Daten anpasst. Die Daten wandern von rechts nach links durchs Bild.
 */
class GraphFixedTime : public Graph {
public:
	/**
	 * @brief Erzeugt einen Graphen mit festem Zeitfenster incl. Channels.
	 * @param name Bezeichnung des Graphen.
	 * @param time Länge das Zeitfensters.
	 * @param channelNames Liste mit den Bezeichnungen der Channels.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	void init(const char *name, unsigned time, std::initializer_list<const char*> channelNames, bool enabled = true);

	/**
	 * @brief Erzeugt einen Graphen mit festem Zeitfenster incl. Channels und Channel-Gruppen.
	 * @param name Bezeichnung des Graphen.
	 * @param time Länge das Zeitfensters.
	 * @param channelNames Liste, die Listen mit den Bezeichnungen der Channels enthält,
	 * entsprechend der gewünschten Zuordnung zu den Channel-Gruppen.
	 * @param channelGroups Liste mit den Bezeichnungen der Channel-Gruppen.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	void init(const char *name, unsigned time, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled = true);

	/**
	 * @brief Fügt einen Channel mit festem Zeitfenster zum Graphen hinzu.
	 * @param title Bezeichnung des Channels.
	 * @param time Länge das Zeitfensters.
	 */
	void addChannel(const char* title, unsigned time);
};

/// \}


} // namespace Debug
} // namespace TURAG



#endif // TINAPP_DEBUG_GRAPH_H
