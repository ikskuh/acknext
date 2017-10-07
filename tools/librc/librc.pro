TEMPLATE = lib
TARGET   = rc

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include($$TOPDIR/extern/gl3w/gl3w.pri)
include($$TOPDIR/extern/json/json.pri)
include($$TOPDIR/extern/stb/stb.pri)

DEPENDPATH += $$TOPDIR/acknext/include/
INCLUDEPATH += $$TOPDIR/acknext/include/

SOURCES += \
    src/image.cpp \
    src/init.cpp \
    src/stb_impl.c \
    src/material.cpp

HEADERS += \
    include/acknext/librc.h \
    src/librc.h
