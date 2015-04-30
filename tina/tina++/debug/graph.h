#ifndef TINAPP_DEBUG_GRAPH_H
#define TINAPP_DEBUG_GRAPH_H

#include <tina++/math.h>
#include <tina++/debug.h>

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
 * Zum Ausgeben von Daten muss eine Instanz einer der Klassen angelegt werden, welche mit einem Titel
 * ein einzelnes Diagramm identifiziert. Jedes Diagramm kann eine beliebige Anzahl von Channels enthalten,
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
 * Instanz und der globalen Ausgabeeinstellung ab.
 *
 * Das Makro \ref TURAG_DEBUG_ENABLE_GRAPH_DEFAULT legt fest, ob die globale Ausgabe standardmäßig aktiviert
 * oder deaktiviert ist.
 *
 * Die Klasse Graph ist ausschließlich zur Verwendung von Subklassen vorgesehen und kann nicht direkt instantiiert werden.
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
#define TURAG_DEBUG_GRAPH_CHANNEL   		"b"
#define TURAG_DEBUG_GRAPH_CHANNEL_FIXED     "B"
#define TURAG_DEBUG_GRAPH_DATA      		"d"
#define TURAG_DEBUG_GRAPH_DATA2D      		"D"
#define TURAG_DEBUG_GRAPH_VERTICAL_MARKER  	"m"
#define TURAG_DEBUG_GRAPH_CHANNELGROUP  	"g"

#endif __DOXYGEN__



/**
 * @brief Basis-Graph-Klasse.
 */
class Graph {
protected:
	/**
	 * @brief Erzeugt einen Graphen.
	 * @param name Bezeichnung des Graphen.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	Graph(const char* name, bool enabled = true);

public:
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
	 * @brief Fügt jedem Channel einen Datensatz hinzu.
	 * @param time Zeit/x-Achse.
	 * @param data Array mit den y-Werten.
	 *
	 * data darf maximal so viele Elemente haben wie der Graph Channels hat.
	 */
	template <size_t N>
	void plot(float time, float& data[N]) {
		doPlot(time, data, N);
    }

	/**
	 * @brief Fügt jedem Channel einen Datensatz hinzu.
	 * @param time Zeit/x-Achse.
	 * @param data Pointer zu den y-Werten.
	 * @param count Anzahl der Werte.
	 *
	 * count darf maximal so groß sein wie die Anzahl der Channels des Graphen.
	 */
	void plot(float time, float* data, size_t count) {
		doPlot(time, data, count);
    }

	/**
	 * @brief Fügt jedem Channel einen Datensatz hinzu.
	 * @param data Array mit x- und y-Werten.
	 *
	 * Mit dieser Funktion kann für jeden Datensatz ein eigener x-Wert übergeben werden, sodass
	 * 2D-Plots möglich sind.
	 *
	 * data enthält x-y-Paare und muss daher stets eine gerade Anzahl von Elementen besitzen.
	 */
	template<size_t N>
	void plot2D(float& data[N]) {
		doPlot2D(data, N);
	}

	/**
	 * @brief Fügt jedem Channel einen Datensatz hinzu.
	 * @param data Array mit x- und y-Werten.
	 * @param pairCount Anzahl der Werte-Paare.
	 *
	 * Mit dieser Funktion kann für jeden Datensatz ein eigener x-Wert übergeben werden, sodass
	 * 2D-Plots möglich sind.
	 *
	 * count darf maximal so viele Elemente umfassen, wie der Graph Channels besitzt. Aus data werden doppelt so
	 * viele Elemente gelesen, wie in count angegeben, da count die Anzahl der Werte-Paare angibt.
	 */
	void plot2D(float* data, size_t pairCount) {
		doPlot2D(data, pairCount);
	}

	/**
	 * @brief Erstellt eine Channelgruppe.
	 * @param label Bezeichnung der Gruppe
	 * @param channelIndices Liste mit den Indices der Channels, die Teil der neuen
	 * Gruppe werden sollen.
	 */
	void addChannelGroup(const char* label, std::initializer_list<unsigned> channelIndices);

protected:
	unsigned index;
	unsigned channels;

private:
	void doPlot(float time, float* data, size_t count);
	void doPlot2D(float* data, size_t count);

	bool enabled;

	static bool allEnabled;
	static std::atomic<unsigned> graphCount;
};


/**
 * @brief Diagramm mit automatischen zoomender Ansicht.
 *
 * Dieses Diagramm zoomt beim Eintreffen neuer Daten so,
 * dass alle Punkte sichtbar sind.
 */
class GraphAuto : public Graph	 {
public:
	/**
	 * @brief Erzeugt einen Graphen.
	 * @param name Bezeichnung des Graphen.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	GraphAuto(const char* name, bool enabled = true) :
		Graph(name, enabled) { }

	/**
	 * @brief Erzeugt einen Graphen incl. Channels.
	 * @param name Bezeichnung des Graphen.
	 * @param channelNames Liste mit den Bezeichnungen der Channels.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	GraphAuto(const char *name, std::initializer_list<const char*> channelNames, bool enabled = true);

	/**
	 * @brief Erzeugt einen Graphen incl. Channels und Channel-Gruppen.
	 * @param name Bezeichnung des Graphen.
	 * @param channelNames Liste, die Listen mit den Bezeichnungen der Channels enthält,
	 * entsprechend der gewünschten Zuordnung zu den Channel-Gruppen.
	 * @param channelGroups Liste mit den Bezeichnungen der Channel-Gruppen.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	GraphAuto(const char *name, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled = true);

	/**
	 * @brief Fügt einen Channel zum Graphen hinzu.
	 * @param title Bezeichnung des Channels.
	 */
	void addChannel(const char* title);
};


/**
 * @brief Diagramm mit festen Ansicht.
 *
 * Dieses Diagramm behält beim Eintreffen neuer Daten die vorgegebene Zoom-Einstellung bei.
 */
class GraphFixedSize : public Graph {
public:
	/**
	 * @brief Erzeugt einen Graphen mit festen Achsen.
	 * @param name Bezeichnung des Graphen.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	GraphFixedSize(const char* name, bool enabled = true) :
		Graph(name, enabled) { }

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
	GraphFixedSize(const char *name, int x_left, int y_bottom, unsigned width, unsigned height, std::initializer_list<const char*> channelNames, bool enabled = true);

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
	GraphFixedSize(const char *name, int x_left, int y_bottom, unsigned width, unsigned height, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled = true);


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
	 * @brief Erzeugt einen Graphen mit festem Zeitfenster.
	 * @param name Bezeichnung des Graphen.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	GraphFixedTime(const char* name, bool enabled = true) :
		Graph(name, enabled) { }

	/**
	 * @brief Erzeugt einen Graphen mit festem Zeitfenster incl. Channels.
	 * @param name Bezeichnung des Graphen.
	 * @param time Länge das Zeitfensters.
	 * @param channelNames Liste mit den Bezeichnungen der Channels.
	 * @param enabled Gibt an, ob die Ausgabe des Graphen standardmäßig
	 * aktiviert ist.
	 */
	GraphFixedTime(const char *name, unsigned time, std::initializer_list<const char*> channelNames, bool enabled = true);

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
	GraphFixedTime(const char *name, unsigned time, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled = true);

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
