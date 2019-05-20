CONFIG   *= c++11

INCLUDEPATH *= $$PWD
DEPENDPATH  *= $$PWD

HEADERS  += \
    $$PWD/tina/tina.h \
    $$PWD/tina++/tina.h \
    $$PWD/tina++/container/array_buffer.h \
    $$PWD/tina++/container/array_ref.h \
    $$PWD/tina++/container/array_storage.h \
    $$PWD/tina++/container/circular_buffer.h \
    $$PWD/tina++/container/container.h \
    $$PWD/tina++/container/queue.h \
    $$PWD/tina++/container/rolling_buffer.h \
    $$PWD/tina++/container/stack.h \
    $$PWD/tina++/container/thread_fifo.h \
    $$PWD/tina++/container/variant_class.h \
    $$PWD/tina++/helper/construct.h \
    $$PWD/tina++/helper/init.h \
    $$PWD/tina++/helper/integer.h \
    $$PWD/tina++/helper/locked.h \
    $$PWD/tina++/helper/macros.h \
    $$PWD/tina++/helper/normalize.h \
    $$PWD/tina++/helper/normalize_type_traits.h \
    $$PWD/tina++/helper/packed.h \
    $$PWD/tina++/helper/scoped_lock.h \
    $$PWD/tina++/helper/static_const.h \
    $$PWD/tina++/helper/traits_algorithms.h \
    $$PWD/tina++/helper/uninitialized.h \
    $$PWD/tina++/range/algorithm.h \
    $$PWD/tina++/utils/flags.h \
    $$PWD/tina++/utils/highresdelaytimer.h \
    $$PWD/tina++/utils/misc-doc.h \
    $$PWD/tina++/utils/timer.h \
    $$PWD/tina++/algorithm.h \
    $$PWD/tina++/math.h \
    $$PWD/tina++/time.h \
    $$PWD/tina/helper/config_tina_default.h \
    $$PWD/tina/helper/locked_type.h \
    $$PWD/tina/helper/macros.h \
    $$PWD/tina/helper/normalize.h \
    $$PWD/tina/helper/static_assert.h \
    $$PWD/tina/helper/types.h \
    $$PWD/tina/utils/bit_macros.h \
    $$PWD/tina/bytes.h \
    $$PWD/tina/math.h \
    $$PWD/tina++/units/config.h \
    $$PWD/tina++/units/math.h \
    $$PWD/tina++/units/unit.h \
    $$PWD/tina++/units/units.h \
    $$PWD/tina++/units/mechanics.h \
    $$PWD/tina++/units/electronics.h \
    $$PWD/tina++/units.h \
    $$PWD/tina++/helper/instance_list.h \
    $$PWD/tina++/statemachine/action.h \
    $$PWD/tina/feldbus/protocol/bldc_protocol.h \
    $$PWD/tina++/ros/time.h \
    $$PWD/tina++/ros/convert.h \
    $$PWD/tina++/geometry/riemanangle.h

DISTR_FILES += $$PWD/tina.pri

#
# debug

contains(TINA, debug) {
  SOURCES += \
      $$PWD/tina++/debug/graph.cpp \
      $$PWD/tina++/debug/errorlogger.cpp \
      $$PWD/tina++/debug/errorobserver.cpp \
      $$PWD/tina/debug/binary.cpp \
      $$PWD/tina/debug/print.cpp \
      $$PWD/tina/debug/image.c

  HEADERS  += \
      $$PWD/tina++/debug/binary.h \
      $$PWD/tina++/debug/errorlogger.h \
      $$PWD/tina++/debug/errorobserver.h \
      $$PWD/tina++/debug/graph.h \
      $$PWD/tina/debug/binary.h \
      $$PWD/tina/debug/defines.h \
      $$PWD/tina/debug/game_time.h \
      $$PWD/tina/debug/image.h \
      $$PWD/tina/debug/internal-debug.h \
      $$PWD/tina/debug/log-source.h \
      $$PWD/tina/debug/print-impl.h \
      $$PWD/tina/debug/print.h \
      $$PWD/tina++/debug.h \
      $$PWD/tina/debug.h
}

#
# geometry

contains(TINA, geometry) {
  SOURCES += \
      $$PWD/tina++/geometry/circle.cpp \
      $$PWD/tina++/geometry/fieldpose.cpp \
      $$PWD/tina++/geometry/geometry.cpp \
      $$PWD/tina++/geometry/pie.cpp \
      $$PWD/tina++/geometry/rect.cpp \
      $$PWD/tina++/geometry/spline.cpp

  HEADERS  += \
      $$PWD/tina++/geometry/circle.h \
      $$PWD/tina++/geometry/fieldpose.h \
      $$PWD/tina++/geometry/geometry.h \
      $$PWD/tina++/geometry/pie.h \
      $$PWD/tina++/geometry/rect.h \
      $$PWD/tina++/geometry/spline.h \
      $$PWD/tina++/geometry/units.h \
      $$PWD/tina++/geometry.h
}

#
# base64

contains(TINA, base64) {
  SOURCES += \
      $$PWD/tina/utils/base64.c

  HEADERS  += \
      $$PWD/tina/utils/base64.h \
      $$PWD/tina++/utils/base64.h
}

#
# crc

contains(TINA, crc) {
  SOURCES += \
      $$PWD/tina/crc/crc_checksum.c

  HEADERS  += \
      $$PWD/tina++/crc/crc.h \
      $$PWD/tina++/crc/xor.h \
      $$PWD/tina++/crc.h \
      $$PWD/tina/crc/crc_checksum.h \
      $$PWD/tina/crc/xor_checksum.h
}

#
# extra

contains(TINA, extra) {
  SOURCES += \
      $$PWD/tina++/utils/cost.cpp

  HEADERS  += \
      $$PWD/tina++/utils/cost.h
}

#
# statemachine

contains(TINA, statemachine) {
  SOURCES += \
      $$PWD/tina++/statemachine/eventqueue.cpp

  HEADERS  += \
      $$PWD/tina/statemachine/eventqueue.h \
      $$PWD/tina++/statemachine/eventqueue.h \
      $$PWD/tina++/statemachine/types.h \
      $$PWD/tina++/statemachine.h
}

#
# feldbus-host

contains(TINA, feldbus-host) {
  SOURCES += \
      $$PWD/tina++/feldbus/host/aktor_tina.cpp \
      $$PWD/tina++/feldbus/host/aseb_tina.cpp \
      $$PWD/tina++/feldbus/host/bootloader_tina.cpp \
      $$PWD/tina++/feldbus/host/dcmotor_tina.cpp \
      $$PWD/tina++/feldbus/host/device_tina.cpp \
      $$PWD/tina++/feldbus/host/farbsensor_tina.cpp \
      $$PWD/tina++/feldbus/host/servo_tina.cpp \
      $$PWD/tina++/feldbus/host/feldbusabstraction.cpp

  HEADERS  += \
      $$PWD/tina++/feldbus/host/aktor.h \
      $$PWD/tina++/feldbus/host/aseb.h \
      $$PWD/tina++/feldbus/host/bootloader.h \
      $$PWD/tina++/feldbus/host/dcmotor.h \
      $$PWD/tina++/feldbus/host/device.h \
      $$PWD/tina++/feldbus/host/farbsensor.h \
      $$PWD/tina++/feldbus/host/sensor.h \
      $$PWD/tina++/feldbus/host/servo.h \
      $$PWD/tina++/feldbus/host/feldbusabstraction.h
}

#
# feldbus-dynamixel

contains(TINA, feldbus-dynamixel) {
  SOURCES += \
      $$PWD/tina++/feldbus/dynamixel/dynamixeldevice.cpp \
      $$PWD/tina++/feldbus/dynamixel/dxl_hal.cpp \
      $$PWD/tina++/feldbus/dynamixel/dynamixel.c

  HEADERS  += \
      $$PWD/tina++/feldbus/dynamixel/dxl_hal.h \
      $$PWD/tina++/feldbus/dynamixel/dynamixel.h \
      $$PWD/tina++/feldbus/dynamixel/dynamixeldevice.h
}

#
# feldbus-protocol

contains(TINA, feldbus-protocol) {
  HEADERS  += \
      $$PWD/tina/feldbus/protocol/turag_feldbus_bus_protokoll.h \
      $$PWD/tina/feldbus/protocol/turag_feldbus_fuer_aseb.h \
      $$PWD/tina/feldbus/protocol/turag_feldbus_fuer_bootloader.h \
      $$PWD/tina/feldbus/protocol/turag_feldbus_fuer_escon.h \
      $$PWD/tina/feldbus/protocol/turag_feldbus_fuer_lokalisierungssensoren.h \
      $$PWD/tina/feldbus/protocol/turag_feldbus_fuer_stellantriebe.h
}

#
# im

contains(TINA, im) {
  SOURCES += \
      $$PWD/tina++/im/statemachine.cpp

  HEADERS  += \
      $$PWD/tina++/im/statemachine.h
}

#
# bluetooth

contains(TINA, bluetooth) {
  SOURCES += \
      $$PWD/tina++/bluetooth/bluetooth_base.cpp

  HEADERS  += \
      $$PWD/tina++/bluetooth/bluetooth_base.h
}

#
# ros

contains(TINA, ros) {
  HEADERS  += \
    $$PWD/tina++/ros/pose.h
}

SOURCES += \
    $$PWD/tina++/statemachine/action.cpp \
    $$PWD/tina++/geometry/riemanangle.cpp

DISTFILES += \
    $$PWD/tina.cmake



