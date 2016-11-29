/*
 * feldbus-driver.cpp
 *
 *  Created on: 20.11.2016
 *      Author: martin
 */

#include <tina/tina.h>

#if TURAG_USE_TURAG_FELDBUS_SLAVE


#include <ch.h>
#include <hal.h>

#include <tina++/feldbus_slave_driver.h>
#include <tina++/feldbus/slave/feldbus_slave_base.h>


namespace TURAG {
namespace Feldbus {
namespace Slave {



#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
    BinarySemaphore Driver::tx_sem;
    BinarySemaphore Driver::tx_finished;
#endif

BinarySemaphore Driver::rx_sem;

Driver::HardwareConfig* Driver::config = nullptr;
Driver::Data Driver::data = {
		0,
		false, false,
		{0}, {0},
		0,
		false,
};




void Driver::init(HardwareConfig* config_) {
    config = config_;

    config->uartConfig.txend2_cb = txComplete;
    config->uartConfig.rxchar_cb = rxChar;
    config->uartConfig.rxerr_cb = rxErr;

    config->gptConfig.callback = rxComplete;

#if TURAG_FELDBUS_SLAVE_CONFIG_ADDRESS_LENGTH == 1
    data.txbuf[0] = TURAG_FELDBUS_MASTER_ADDR|MY_ADDR;
#elif TURAG_FELDBUS_SLAVE_CONFIG_ADDRESS_LENGTH == 2
    data.txbuf[0] = (TURAG_FELDBUS_MASTER_ADDR_2|MY_ADDR) & 0xff;
    data.txbuf[1] = (TURAG_FELDBUS_MASTER_ADDR_2|MY_ADDR) >> 8;
#endif
    data.timerTicks = (config->gptConfig.frequency * 15 + config->baudrate / 2) / config->baudrate;


    disableRts();
    if (config->ledInverted) {
        palSetPad(config->ledPort, config->ledPin);
    } else {
        palClearPad(config->ledPort, config->ledPin);
    }
    
#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
    chBSemInit(&tx_sem, false);
    chBSemInit(&tx_finished, true);
#endif
    chBSemInit(&rx_sem, true);
}


void Driver::start(ThreadImpl* thread, int prio) {
    thread->start(prio, &thread_func);
    gptStart(config->gptp, &config->gptConfig);
    uartStart(config->uartp, &config->uartConfig);
}


#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
void Driver::transmitDebugData(const void* data, size_t length) {
    chBSemWait(&tx_sem);
    enableRts();
    uartStartSend(config->uartp, length, data);
    chBSemWait(&tx_finished);
}
#endif


void Driver::thread_func() {
    chRegSetThreadName("feldbus slave driver");

    while(1) {
        chSysLock();
        // Reset packet data after it was processed
        // at the end of this function. We do this here
        // to save a call to chSysLockFromISR().
        if (data.processing) {
            data.processing = false;
            data.packet_complete = false;
            data.rx_size = 0;
        }

        // sleep until there is a packet waiting to be processed
        // or we need to toggle our led.
        chBSemWaitTimeoutS(&rx_sem, MS2ST(20));

        if (data.packet_complete) {
            data.processing = true;
        }
        chSysUnlock();

#if TURAG_FELDBUS_SLAVE_CONFIG_FLASH_LED
        Base::doLedPattern(50);
#endif

        if (data.processing) {
            FeldbusSize_t length = Base::processPacket(data.rxbuf, data.rx_size, data.txbuf);
            if (length > 0) {
#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
                chBSemWait(&tx_sem);
#endif
                enableRts();
                uartStartSend(config->uartp, length, data.txbuf);
            }
        }
    }
}


// new receiption has started
void Driver::rxChar(UARTDriver *, uint16_t c) {
	chSysLockFromIsr();
	if (!data.processing) {
		if (data.packet_complete) {
			// packet was ready, but there is a new one before
			// it was processed -> dump it and receive new one
			data.rx_size = 0;
			data.packet_complete = false;
#if (TURAG_FELDBUS_SLAVE_CONFIG_PACKAGE_STATISTICS_AVAILABLE)
			Base::increasePacketLost();
#endif
		}

		if (data.rx_size == TURAG_FELDBUS_SLAVE_CONFIG_BUFFER_SIZE) {
			// buffer overflow
			data.overflow = true;
		} else {
			data.rxbuf[data.rx_size] = static_cast<uint8_t>(c);
			++data.rx_size;
		}
		// restart timer to wait for more arriving data
		if (config->gptp->state != GPT_READY) gptStopTimerI(config->gptp);
		gptStartOneShotI(config->gptp, data.timerTicks);
	} else {
		// Byte received, but package is processing -> ignore.
		// This shouldn't happen if the data is really meant
		// for us.
	}
	chSysUnlockFromIsr();
}


void Driver::rxComplete(GPTDriver *) {
	chSysLockFromIsr();

	if (packetAdressedToMe()) {
		if (!data.overflow && data.rx_size > 1) {
			// valid packet, addressed to us
			data.packet_complete = true;
			// wake up worker thread
			chBSemSignalI(&rx_sem);
		} else if (data.overflow) {
			data.overflow = false;
#if (TURAG_FELDBUS_SLAVE_CONFIG_PACKAGE_STATISTICS_AVAILABLE)
			Base::increaseBufferOverflow();
#endif
		}
	}
	if (!data.packet_complete) {
		data.rx_size = 0;
	}

	chSysUnlockFromIsr();
}


// usart hardware error detected
void Driver::rxErr(UARTDriver *, uartflags_t) {
	// TODO: What?
}


// dma transmit complete
void Driver::txComplete(UARTDriver *) {
    disableRts();
#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
    chSysLockFromIsr();
    chBSemSignalI(&tx_sem);
    chBSemSignalI(&tx_finished);
    chSysUnlockFromIsr();
#endif
}


}
}
}


#endif // TURAG_USE_TURAG_FELDBUS_SLAVE
