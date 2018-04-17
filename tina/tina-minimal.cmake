cmake_minimum_required(VERSION 2.8.3)

set(TINA_INCLUDE_DIRS .)

set(TINA_SRC
  tina++/geometry/circle.cpp
  tina++/geometry/geometry.cpp
  tina++/geometry/spline.cpp
  tina++/geometry/rect.cpp
  tina++/geometry/pie.cpp
  tina/debug/graph.cpp
  tina/debug/print.cpp
  tina/debug/binary.cpp
  tina/crc/crc_checksum.c
  tina/debug/image.c
  tina/utils/base64.c)

PREPEND_PATH(TINA_SRC)
PREPEND_PATH(TINA_INCLUDE_DIRS)
