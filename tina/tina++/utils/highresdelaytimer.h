#ifndef TINAPP_CHIBIOS_HIGHRESDELAYTIMER_H
#define TINAPP_CHIBIOS_HIGHRESDELAYTIMER_H

#include <tina++/private/highresdelaytimer_private.h>

#if !TURAG_USE_TINA_HIGH_RES_DELAY_TIMER
# error TURAG_USE_TINA_HIGH_RES_DELAY_TIMER must be defined to 1
#endif

namespace TURAG {
	
/**
 * \ingroup Misc
 * \brief Pausiert Threads im Mikrosekunden-Bereich.
 * 
 * Diese Klasse stellt Funktionalität bereit, um einen Thread für
 * einen Zeitraum zu pausieren, der kürzer als ein System-Tick ist.
 *
 * Die Implementierung ist plattformabhängig, das Verhalten kann
 * sich dadurch unterscheiden.
 *
 * \note Es ist zu beachten, dass sich die Verzögerung im
 * Thread-Kontext abspielt. Dadurch können die gewünschten
 * Verzögerungen nur eine Mindestverzögerung sein. Speziell wenn der
 * verzögerte %Thread nicht die höchste Priorität besitzt, kann
 * die tatsächliche Verzögerung beliebig lang werden.
 * 
 * \note Diese Implementierung ist reentrant, jedoch nicht thread-safe!
 * Jede Instanz kann gleichzeitig von nur einem Thread zum warten benutzt
 * werden.
 *
 */
class HighResDelayTimer {
public:
	/**
	 * \brief Konstruktor
	 */
    HighResDelayTimer(void);
	
	/**
	 * \brief Destruktor
	 */
    ~HighResDelayTimer(void);
	
	/**
	 * @brief Initialisiert den DelayTimer.
	 */
	void init(void);
	
	/**
	 * @brief Blockiert den Thread um die angegebene Zeit.
	 * @param us Zeit in Mikrosekunden.
	 */
	void wait_us(uint32_t us);

    /**
     * @brief Startet den Timer.
     * @param us Zeit in Mikrosekunden.
     *
     * Diese Funktion startet den Timer mit der gewünschten Zeit
     * und kehrt sofort zurück. isRunning() kann benutzt werden
     * um zu prüfen, ob der Timer schon abgelaufen ist.
     */
    void startTimeout_us(uint32_t us);

    /**
     * @brief Prüft ob der Timer läuft.
     * @return True, wenn der Timer läuft, ansonsten false.
     *
     * Diese Funktion kann nach einem Aufruf von startTimeout_us()
     * benutzt werden um zu prüfen, ob die entsprechende Zeit bereits
     * vergangen ist.
     */
    bool isRunning(void);
	
private:
	// storage space for implementations
	HighResDelayTimerPrivate data;
};

} // namespace TURAG

#endif // TINAPP_CHIBIOS_HIGHRESDELAYTIMER_H
