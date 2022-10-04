#ifndef PLATFORM_CHIBIOS3_PUBLIC_TINAPP_PIN_CONFIG_H
#define PLATFORM_CHIBIOS3_PUBLIC_TINAPP_PIN_CONFIG_H

#include <cstddef>
#include <utility>

#include <hal.h>

namespace TURAG {

enum class Port : uint8_t {
    A = 0x00,
    B = 0x10,
    C = 0x20,
    D = 0x30,
    E = 0x40,
    F = 0x50,
    G = 0x60,
    H = 0x70,
    I = 0x80,
    J = 0x90,
    K = 0xA0,
};

enum class Mode : uint8_t {
    INPUT = 0,
    OUTPUT = 1,
    ALTERNATE = 2,
    ANALOG = 3
};

enum class OutputData : uint8_t {
    LOW = 0,
    HIGH = 1
};

enum class OutputType : uint8_t {
    PUSHPULL = 0,
    OPENDRAIN = 1
};

enum class OutputSpeed : uint8_t {
    VERYLOW = 0,
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3
};

enum class PullupPulldown : uint8_t {
    NONE = 0,
    PULLUP = 1,
    PULLDOWN = 2
};

struct PinConfig {
    constexpr PinConfig(Mode mode_, OutputData odata_, OutputType otype_, OutputSpeed ospeed_,
                        PullupPulldown pupd_, uint8_t af_)
        : mode(static_cast<uint32_t>(mode_)),
          odata(static_cast<uint32_t>(odata_)),
          otype(static_cast<uint32_t>(otype_)),
          ospeed(static_cast<uint32_t>(ospeed_)),
          pupd(static_cast<uint32_t>(pupd_)),
          af(af_) {}
    uint32_t mode;
    uint32_t odata;
    uint32_t otype;
    uint32_t ospeed;
    uint32_t pupd;
    uint32_t af;
};

constexpr PinConfig InputPin(PullupPulldown pupd) {
    return PinConfig(Mode::INPUT, OutputData::HIGH, OutputType::PUSHPULL, OutputSpeed::HIGH, pupd, 0);
}

constexpr PinConfig OutputPin(OutputData od, OutputType otype, OutputSpeed ospeed, PullupPulldown pupd) {
    return PinConfig(Mode::OUTPUT, od, otype, ospeed, pupd, 0);
}

constexpr PinConfig AnalogPin() {
    return PinConfig(Mode::ANALOG, OutputData::HIGH, OutputType::PUSHPULL, OutputSpeed::HIGH, PullupPulldown::NONE, 0);
}

constexpr PinConfig AlternatePin(OutputType otype, PullupPulldown pupd, uint8_t af) {
    return PinConfig(Mode::ALTERNATE, OutputData::HIGH, otype, OutputSpeed::HIGH, pupd, af);
}

class Pin {
public:
    // Representation usable in switch statements that does not convert to int
    enum class Raw : uint8_t {};

    explicit constexpr Pin(Port port, uint8_t pin)
        : raw(static_cast<Raw>((static_cast<uint8_t>(port)) | pin)) {
    }

    constexpr Port port() const {
        return static_cast<Port>(static_cast<uint8_t>(raw) & 0xF0);
    }

    constexpr uint8_t pad() const {
        return static_cast<uint8_t>(static_cast<uint8_t>(raw) & 0x0F);
    }

    explicit operator ioline_t() const {
        switch (port()) {
#if STM32_HAS_GPIOA
        case Port::A: return PAL_LINE(GPIOA, pad());
#endif
#if STM32_HAS_GPIOB
        case Port::B: return PAL_LINE(GPIOB, pad());
#endif
#if STM32_HAS_GPIOC
        case Port::C: return PAL_LINE(GPIOC, pad());
#endif
#if STM32_HAS_GPIOD
        case Port::D: return PAL_LINE(GPIOD, pad());
#endif
#if STM32_HAS_GPIOE
        case Port::E: return PAL_LINE(GPIOE, pad());
#endif
#if STM32_HAS_GPIOF
        case Port::F: return PAL_LINE(GPIOF, pad());
#endif
#if STM32_HAS_GPIOG
        case Port::G: return PAL_LINE(GPIOG, pad());
#endif
#if STM32_HAS_GPIOH
        case Port::H: return PAL_LINE(GPIOH, pad());
#endif
#if STM32_HAS_GPIOI
        case Port::I: return PAL_LINE(GPIOI, pad());
#endif
#if STM32_HAS_GPIOJ
        case Port::J: return PAL_LINE(GPIOJ, pad());
#endif
#if STM32_HAS_GPIOK
        case Port::K: return PAL_LINE(GPIOK, pad());
#endif
        // silence compiler about unused enum values for GPIO ports the MCU does not have
        default: return 0;
        }
    }

    constexpr operator Raw() const {
        return raw;
    }

private:
    // 8 bit internal representation
    // 4 MSB: port
    // 4 LSB: pin
    Raw raw;
};

static constexpr uint32_t bitwise_or() {
    return 0;
}

template <typename... Ts>
static constexpr uint32_t bitwise_or(uint32_t first, Ts... next) {
    return first | bitwise_or(next...);
}

template <PinConfig (*pin_cfg)(Pin), Port port, std::size_t... pins>
static constexpr uint32_t all_mode_bits(std::index_sequence<pins...>) {
    return bitwise_or(pin_cfg(Pin(port, pins)).mode << (pins * 2U)...);
}

template <PinConfig (*pin_cfg)(Pin), Port port, std::size_t... pins>
static constexpr uint32_t all_output_data_bits(std::index_sequence<pins...>) {
    return bitwise_or(pin_cfg(Pin(port, pins)).odata << (pins * 1U)...);
}

template <PinConfig (*pin_cfg)(Pin), Port port, std::size_t... pins>
static constexpr uint32_t all_output_type_bits(std::index_sequence<pins...>) {
    return bitwise_or(pin_cfg(Pin(port, pins)).otype << (pins * 1U)...);
}

template <PinConfig (*pin_cfg)(Pin), Port port, std::size_t... pins>
static constexpr uint32_t all_output_speed_bits(std::index_sequence<pins...>) {
    return bitwise_or(pin_cfg(Pin(port, pins)).ospeed << (pins * 2U)...);
}

template <PinConfig (*pin_cfg)(Pin), Port port, std::size_t... pins>
static constexpr uint32_t all_pullup_pulldown_bits(std::index_sequence<pins...>) {
    return bitwise_or(pin_cfg(Pin(port, pins)).pupd << (pins * 2U)...);
}

template <PinConfig (*pin_cfg)(Pin), Port port, std::size_t... pins>
static constexpr uint32_t low_alternate_function_bits(std::index_sequence<pins...>) {
    return bitwise_or(pin_cfg(Pin(port, pins)).af << (pins * 4U)...);
}

template <PinConfig (*pin_cfg)(Pin), Port port, std::size_t... pins>
static constexpr uint32_t high_alternate_function_bits(std::index_sequence<pins...>) {
    return bitwise_or(pin_cfg(Pin(port, pins + 8)).af << (pins * 4U)...);
}


//having the values as template parameters enforces computation of bitfields at compile-time
template <uint32_t mode_bits, uint32_t odr, uint32_t otyper, uint32_t ospeedr, uint32_t pupdr, uint32_t afrl, uint32_t afrh>
static void configure_port(stm32_gpio_t* gpio) {
    gpio->MODER = mode_bits;
    gpio->ODR = odr;
    gpio->OTYPER = otyper;
    gpio->OSPEEDR = ospeedr;
    gpio->PUPDR = pupdr;
    gpio->AFRL = afrl;
    gpio->AFRH = afrh;
}

template <PinConfig (*pin_cfg)(Pin), Port port>
static void configure_port(stm32_gpio_t* gpio) {
    configure_port<
        all_mode_bits<pin_cfg, port>(std::make_index_sequence<16>{}),
        all_output_data_bits<pin_cfg, port>(std::make_index_sequence<16>{}),
        all_output_type_bits<pin_cfg, port>(std::make_index_sequence<16>{}),
        all_output_speed_bits<pin_cfg, port>(std::make_index_sequence<16>{}),
        all_pullup_pulldown_bits<pin_cfg, port>(std::make_index_sequence<16>{}),
        low_alternate_function_bits<pin_cfg, port>(std::make_index_sequence<8>{}),
        high_alternate_function_bits<pin_cfg, port>(std::make_index_sequence<8>{})>(gpio);
}

// function to be called by initialization code
template <PinConfig (*pin_cfg)(Pin)>
static void configure_gpio() {
#if STM32_HAS_GPIOA
    configure_port<pin_cfg, Port::A>(GPIOA);
#endif
#if STM32_HAS_GPIOB
    configure_port<pin_cfg, Port::B>(GPIOB);
#endif
#if STM32_HAS_GPIOC
    configure_port<pin_cfg, Port::C>(GPIOC);
#endif
#if STM32_HAS_GPIOD
    configure_port<pin_cfg, Port::D>(GPIOD);
#endif
#if STM32_HAS_GPIOE
    configure_port<pin_cfg, Port::E>(GPIOE);
#endif
#if STM32_HAS_GPIOF
    configure_port<pin_cfg, Port::F>(GPIOF);
#endif
#if STM32_HAS_GPIOG
    configure_port<pin_cfg, Port::G>(GPIOG);
#endif
#if STM32_HAS_GPIOH
    configure_port<pin_cfg, Port::H>(GPIOH);
#endif
#if STM32_HAS_GPIOI
    configure_port<pin_cfg, Port::I>(GPIOI);
#endif
#if STM32_HAS_GPIOJ
    configure_port<pin_cfg, Port::J>(GPIOJ);
#endif
#if STM32_HAS_GPIOK
    configure_port<pin_cfg, Port::K>(GPIOK);
#endif
}

// sensible default pin config for all known STM32
static constexpr PinConfig default_pin_cfg(Pin pin) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch (pin) {
    case Pin(Port::A, 13): return AlternatePin(OutputType::PUSHPULL, PullupPulldown::PULLDOWN, 0);
    case Pin(Port::A, 14): return AlternatePin(OutputType::PUSHPULL, PullupPulldown::PULLDOWN, 0);
    default: return InputPin(PullupPulldown::NONE);
    }
#pragma GCC diagnostic pop
}

} // namespace TURAG

#endif // PLATFORM_CHIBIOS3_PUBLIC_TINAPP_PIN_CONFIG_H
