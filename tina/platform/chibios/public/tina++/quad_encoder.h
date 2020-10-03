#ifndef TURAG_QUAD_ENCODER_H
#define TURAG_QUAD_ENCODER_H

#include <hal.h>
#include <tina++/time.h>
#include <tina++/units.h>
#include <tina++/units/mechanics.h>

namespace TURAG {

///
/// \brief Klasse zur Verwendung von Quadratur-Encodern mit STM32-Timern
///
class QuadEncoder {
public:
    using Increments = int16_t;
    using IncrementsDiff = int32_t;

    enum class InvertDirection {
        NO_INVERT,
        INVERT,
    };

    struct Config {
        ///
        /// \brief STM32-Timer, der für den Encoder verwendet werden soll
        /// 
        /// Es werden nur die Timer 2, 3, 4 und 5 unterstützt.
        ///
        stm32_tim_t* timer;

        ///
        /// \brief Gibt an, ob die Drehrichtung invertiert werden soll
        ///
        InvertDirection invert;

        ///
        /// \brief Anzahl der Inkremente für eine ganze Radumdrehung
        ///
        Increments total_increments;
    };

    ///
    /// \brief Quadratur-Encoder-Klasse anlegen
    /// \param config Konfiguration der Quadratur-Encoder
    ///
    constexpr QuadEncoder(const Config& config) : config_(config) {}

    ///
    /// \brief STM32-Timer für Verwendung mit Quadratur-Encoder initialisieren.
    /// 
    /// Muss vor der ersten Verwendung aufgerufen werden!
    ///
    void init();

    ///
    /// \brief Messung durchführen.
    /// 
    /// Die Verwendung von getMeasurementTime(), getWheelAngleStep() sowie getWheelSpeed()
    /// erfordert eine periodische Durchführung von Messungen.
    ///
    void measure();

    ///
    /// \brief Zählregister des Timers ausgeben
    /// \return Anzahl der Inkremente
    ///
    Increments getIncrements() const;

    ///
    /// \brief Gibt an, ob eine valide Messung vorliegt.
    /// \return Ob valide Messung vorliegt
    ///
    constexpr bool hasValidMeasurement() const {
        return last_timediff_ != SystemTime();
    }

    ///
    /// \brief Gibt den Zeitpunkt der letzten Messung aus
    /// \return Zeitpunkt der letzten Messung
    ///
    constexpr SystemTime getMeasurementTime() const {
        return last_timestamp_;
    }

    ///
    /// \brief Gibt Winkeldifferenz seit der letzten Messung aus
    /// \return Winkeldifferenz seit der letzten Messung
    ///
    constexpr Units::Angle getWheelAngleStep() const {
        return hasValidMeasurement()
                ? Units::Real(increment_diff_) / Units::Real(config_.total_increments) * Units::round
                : Units::null;
    }

    ///
    /// \brief Gibt momentane Drehrate des Rads aus
    /// \return Drehrate des Rads
    ///
    constexpr Units::AngularVelocity getWheelSpeed() const {
        return hasValidMeasurement()
                ? getWheelAngleStep() / last_timediff_.toTime()
                : Units::null;
    }

private:
    Config config_;

    SystemTime last_timestamp_, last_timediff_;
    Increments increment_count_ = 0;
    IncrementsDiff increment_diff_ = 0;
};

}  // namespace TURAG

#endif // TURAG_QUAD_ENCODER_H
