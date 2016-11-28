INCLUDEPATH += $$PWD/public

SOURCES += \
    $$PWD/thread.cpp \
    $$PWD/debugprint.cpp

HEADERS  += \
    $$PWD/public/tina/can.h \
    $$PWD/public/tina/config_tina_platform.h \
    $$PWD/public/tina/debugprint.h \
    $$PWD/public/tina/thread.h \
    $$PWD/public/tina/time.h \
    $$PWD/public/tina/timetype.h \
    $$PWD/public/tina++/can.h \
    $$PWD/public/tina++/thread.h

DISTR_FILES += $$PWD/tina-desktop.pri
