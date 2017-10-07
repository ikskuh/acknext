TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


SOURCES += \
    process-material.cpp \
    main.cpp \
    process-texture.cpp

include($$TOPDIR/acknext/acknext.pri)
include($$TOPDIR/extern/json/json.pri)

# Use librc
LIBS += -lrc
INCLUDEPATH += $$PWD/../librc/include
DEPENDPATH += $$PWD/../librc/include
