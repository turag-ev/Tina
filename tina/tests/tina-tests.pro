#-------------------------------------------------
#
# Project created by QtCreator 2016-07-01T21:35:53
#
#-------------------------------------------------

QT       += testlib

#QT       -= gui

DEFINES += SIM SIMULATION BOT_A TURAG_NO_PROJECT_CONFIG TURAG_DEBUG_ENABLE_BINARY

TARGET = tina-tests
CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app

CONFIG += c++14

QMAKE_CXXFLAGS += -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast
QMAKE_CXXFLAGS += -Wcast-align -Wunused -Winit-self -Woverloaded-virtual
QMAKE_CXXFLAGS += -Wno-unused-parameter

# Boost test
LIBS += -lboost_unit_test_framework

SOURCES += \
    ../platform/qt/debugprint.cpp \
    ../platform/qt/time.cpp \
    main.cpp \
    container_test.cpp \
    container_test_test.cpp \
    state_tests.cpp \
    math_tests.cpp \
    units_tests.cpp \
    macro_tests.cpp \
    geometry_tests.cpp \
    circular_buffer_tests.cpp \
    bit_macros_tests.cpp \
    array_buffer_tests.cpp \
    helper/variant_class_tests.cpp

HEADERS += \
    container_test.h \
    extentions.h

INCLUDEPATH += \
    ../platform/qt/public

TINA += debug statemachine geometry base64

include(../../src/common/TURAG-common.pri)
include(../tina.pri)
