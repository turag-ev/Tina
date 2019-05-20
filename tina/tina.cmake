cmake_minimum_required(VERSION 2.8.3)

set(TINA_INCLUDE_DIRS .)

set(TINA_SRC
  tina++/statemachine/action.cpp
  tina++/statemachine/eventqueue.cpp
  tina++/feldbus/host/dcmotor_tina.cpp
  tina++/feldbus/host/servo_tina.cpp
  tina++/feldbus/host/bootloader_tina.cpp
  tina++/feldbus/host/aktor_tina.cpp
  tina++/feldbus/host/device_tina.cpp
  tina++/feldbus/host/feldbusabstraction.cpp
  tina++/feldbus/host/aseb_tina.cpp
  tina++/feldbus/host/farbsensor_tina.cpp
  tina++/feldbus/host/stellantriebedevice.cpp
  tina++/feldbus/dynamixel/dxl_hal.cpp
  tina++/feldbus/dynamixel/dynamixeldevice.cpp
  tina++/feldbus/dynamixel/dynamixel.c
  tina++/geometry/circle.cpp
  tina++/geometry/fieldpose.cpp
  tina++/geometry/geometry.cpp
  tina++/geometry/spline.cpp
  tina++/geometry/rect.cpp
  tina++/geometry/pie.cpp
  tina++/debug/errorobserver.cpp
  tina++/debug/errorlogger.cpp
  tina++/debug/graph.cpp
  tina++/bluetooth/bluetooth_base.cpp
  tina++/utils/cost.cpp
  tina++/im/statemachine.cpp
  tina/debug/print.cpp
  tina/debug/binary.cpp
  tina/crc/crc_checksum.c
  tina/debug/image.c
  tina/utils/base64.c)

PREPEND_PATH(TINA_SRC)
PREPEND_PATH(TINA_INCLUDE_DIRS)
