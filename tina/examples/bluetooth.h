#ifndef SYSTEMCONTROL_COMMON_BLUETOOTH_H
#define SYSTEMCONTROL_COMMON_BLUETOOTH_H

//! [Includes]
#include <tina++/tina.h>
#include <tina/bluetooth_config.h>
#include <tina++/bluetooth/bluetooth_base.h>
//! [Includes]


//! [class]

namespace TURAG {

class Bluetooth : public BluetoothBase {
public:
    static Bluetooth* get(void) {
        return &instance_;
    }

protected:
    Bluetooth(void) { }

    virtual void lowlevelInit(void);
    virtual bool write(uint8_t peer_id, uint8_t* buffer, size_t buffer_size);
    virtual Status getConnectionStatusLowlevel(uint8_t peer_id);
    virtual void setPeerEnabledLowlevel(uint8_t peer_id, bool enabled);


private:
    static Bluetooth instance_;
};


} // namespace TURAG
//! [class]




#endif // SYSTEMCONTROL_COMMON_BLUETOOTH_H
