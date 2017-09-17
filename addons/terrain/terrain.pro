TEMPLATE = lib
CONFIG += c++11
# CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

SOURCES += \
    l3dt/heightfield.c \
    l3dt/attributefield.c \
    terrainmodule.c

HEADERS += \
    l3dt/l3dt.h \
    terrainmodule.h
