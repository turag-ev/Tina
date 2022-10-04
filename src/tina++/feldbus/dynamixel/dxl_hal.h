#ifndef TINAPP_FELDBUS_DYNAMIXEL_DXL_HAL_H
#define TINAPP_FELDBUS_DYNAMIXEL_DXL_HAL_H


#ifdef __cplusplus
extern "C" {
#endif


int turag_dxl_hal_open(void *feldbusAbstractionInstance);
void turag_dxl_hal_close(void);
int turag_dxl_hal_set_baud( float baudrate );
void turag_dxl_hal_clear(void);
int turag_dxl_hal_tx( unsigned char *pPacket, int numPacket );
int turag_dxl_hal_rx( unsigned char *pPacket, int numPacket );
void turag_dxl_hal_set_timeout( int NumRcvByte );
int turag_dxl_hal_timeout(void);



#ifdef __cplusplus
}
#endif

#endif // TINAPP_FELDBUS_DYNAMIXEL_DXL_HAL_H
