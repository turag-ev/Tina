#ifndef PIN_CONFIG_H_
#define PIN_CONFIG_H_
#include <config_tina.h>

#define PINCONFIG_INPUT(port, pin, pupdr) \
	PINCONFIG(port, pin, Mode::INPUT, OutputData::HIGH, OutputType::PUSHPULL, OutputSpeed::HIGH, pupdr, 0)

#define PINCONFIG_OUTPUT(port, pin, odr, otype, ospeed, pupdr) \
	PINCONFIG(port, pin, Mode::OUTPUT, OutputData::HIGH, otype, ospeed, pupdr, 0)

#define PINCONFIG_ANALOG(port, pin) \
	PINCONFIG(port, pin, Mode::ANALOG, OutputData::HIGH, OutputType::PUSHPULL, OutputSpeed::HIGH, PullupPulldown::NONE, 0)

#define PINCONFIG_ALTERNATE(port, pin, otype, pupdr, af) \
	PINCONFIG(port, pin, Mode::ALTERNATE, OutputData::HIGH, otype, OutputSpeed::HIGH, pupdr, af)

//generate default settings for all pins
#define DEFAULT_PINCONFIG(mode_, od_, otype_, ospeed_, pupd_, af_) \
	template<Port, unsigned> struct Pin { \
		using cfg = PinConfig<mode_,od_,otype_,ospeed_,pupd_,af_>; \
	}

//override default settings for a specific pin. DEFAULT_PINCONFIG must be used before this
#define PINCONFIG(port, pin, mode_, od_, otype_, ospeed_, pupd_, af_) \
	template<> struct Pin<Port::port, pin> { \
			using cfg = PinConfig<mode_,od_,otype_,ospeed_,pupd_,af_>; \
	}

enum class Mode {
	INPUT = 0,
	OUTPUT = 1,
	ALTERNATE = 2,
	ANALOG = 3
};

enum class OutputData {
	LOW = 0,
	HIGH = 1
};
enum class OutputType {
	PUSHPULL = 0,
	OPENDRAIN = 1
};
enum class OutputSpeed {
	VERYLOW = 0,
	LOW = 1, 
	MEDIUM = 2,
	HIGH = 3
};
enum class PullupPulldown {
	NONE = 0,
	PULLUP = 1,
	PULLDOWN = 2
};

enum class Port { A, B, C, D, E, F, G, H, I};

//template that stores parameters
template<Mode mode_, OutputData od_, OutputType otype_, OutputSpeed ospeed_, PullupPulldown pupd_, unsigned af_>
struct PinConfig {
	static constexpr Mode mode = mode_;
	static constexpr OutputData od = od_;
	static constexpr OutputType otype = otype_;
	static constexpr OutputSpeed ospeed = ospeed_;
	static constexpr PullupPulldown pupd = pupd_;
	static constexpr unsigned af = af_;
};

//convert configuration structures to bits in GPIO configuration registers
#define MODER_BITS(port, pin) \
	(static_cast<unsigned>(Pin<Port::port, pin>::cfg::mode) << ((pin) * 2U))
#define ODR_BITS(port, pin) \
	(static_cast<unsigned>(Pin<Port::port, pin>::cfg::od) << (pin))
#define OTYPER_BITS(port, pin) \
	(static_cast<unsigned>(Pin<Port::port, pin>::cfg::otype) << (pin))
#define OSPEEDR_BITS(port, pin) \
	(static_cast<unsigned>(Pin<Port::port, pin>::cfg::ospeed) << ((pin) * 2U))
#define PUPDR_BITS(port, pin) \
	(static_cast<unsigned>(Pin<Port::port, pin>::cfg::pupd) << ((pin) * 2U))
#define AFR_BITS(port, pin) \
	((Pin<Port::port,pin>::cfg::af) << (((pin) % 8U) * 4U))

//combine the values of x for pins 0 to 7 on a given port
#define PORTL_VAL(port, x) \
 (x(port,0) | 	x(port,1) | 	x(port,2) | 	x(port,3) | \
	x(port,4) | 	x(port,5) | 	x(port,6) | 	x(port,7))
//combine the values of x for pins 8 to 15 on a given port
#define PORTH_VAL(port, x) \
 (x(port,8) | 	x(port,9) | 	x(port,10)| 	x(port,11)| \
	x(port,12)| 	x(port,13)| 	x(port,14)| 	x(port,15))
//combine the values of x for all pins (0 to 15) on a given port
#define PORT_VAL(port, x) \
	(PORTL_VAL(port,x) | PORTH_VAL(port,x))

//generate sensible default pin configuration (should be identical to configuration on reset)
#if !defined(TURAG_NO_DEFAULT_PINCONFIG) || (TURAG_NO_DEFAULT_PINCONFIG != 1)
	//all pins floating input
	DEFAULT_PINCONFIG(Mode::INPUT, OutputData::HIGH, OutputType::PUSHPULL, OutputSpeed::HIGH, PullupPulldown::NONE, 0);
	// PA13 and PA14 are SWDIO and SWCLK programming pins on all known STM32
	PINCONFIG_ALTERNATE(A, 13, OutputType::PUSHPULL, PullupPulldown::PULLUP, 0); //SWDIO
	PINCONFIG_ALTERNATE(A, 14, OutputType::PUSHPULL, PullupPulldown::PULLDOWN, 0); // SWCLK
#endif // TURAG_NO_DEFAULT_PINCONFIG

/*
	ChibiOS PAL specific part
*/
//detect ChibiOS PAL, this requires ChibiOS' hal.h to be included beforehand
#if HAL_USE_PAL 

//generate part of ChibiOS' PALConfig structure for a given port
#define PORT_CONFIG_CHIBIOS(port) {\
	PORT_VAL(port, MODER_BITS), \
	PORT_VAL(port, OTYPER_BITS), \
	PORT_VAL(port, OSPEEDR_BITS), \
	PORT_VAL(port, PUPDR_BITS), \
	PORT_VAL(port, ODR_BITS), \
	PORTL_VAL(port, AFR_BITS), \
	PORTH_VAL(port, AFR_BITS)	}

//define PORTCFG_* for ports wich exist on the device and empty placeholders for ports wich don't
#if STM32_HAS_GPIOA
# define PORTCFG_A PORT_CONFIG_CHIBIOS(A),
#else
# define PORTCFG_A
#endif // STM32_HAS_GPIOA
#if STM32_HAS_GPIOB
# define PORTCFG_B PORT_CONFIG_CHIBIOS(B),
#else
# define PORTCFG_B
#endif // STM32_HAS_GPIOB
#if STM32_HAS_GPIOC
# define PORTCFG_C PORT_CONFIG_CHIBIOS(C),
#else
# define PORTCFG_C
#endif // STM32_HAS_GPIOC
#if STM32_HAS_GPIOD
# define PORTCFG_D PORT_CONFIG_CHIBIOS(D),
#else
# define PORTCFG_D
#endif // STM32_HAS_GPIOD
#if STM32_HAS_GPIOE
# define PORTCFG_E PORT_CONFIG_CHIBIOS(E),
#else
# define PORTCFG_E
#endif // STM32_HAS_GPIOE
#if STM32_HAS_GPIOF
# define PORTCFG_F PORT_CONFIG_CHIBIOS(F),
#else
# define PORTCFG_F
#endif // STM32_HAS_GPIOF
#if STM32_HAS_GPIOG
# define PORTCFG_G PORT_CONFIG_CHIBIOS(G),
#else
# define PORTCFG_G
#endif // STM32_HAS_GPIOG
#if STM32_HAS_GPIOH
# define PORTCFG_H PORT_CONFIG_CHIBIOS(H),
#else
# define PORTCFG_H
#endif // STM32_HAS_GPIOH
#if STM32_HAS_GPIOI
# define PORTCFG_I PORT_CONFIG_CHIBIOS(I)
#else
# define PORTCFG_I
#endif // STM32_HAS_GPIOI

#define DEFINE_PAL_CONFIG() const PALConfig pal_default_config = { \
	PORTCFG_A PORTCFG_B PORTCFG_C PORTCFG_D PORTCFG_E \
	PORTCFG_F PORTCFG_G PORTCFG_H PORTCFG_I }

#endif // HAL_USE_PAL

#endif // PIN_CONFIG_H_