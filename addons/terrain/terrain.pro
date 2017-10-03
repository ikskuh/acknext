TEMPLATE = lib
CONFIG += c++11
# CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

include($$TOPDIR/utilities/resource-compiler/resource-compiler.pri)
include($$TOPDIR/acknext/acknext.pri)

SOURCES += \
    l3dt/heightfield.c \
    l3dt/attributefield.c \
    terrainmodule.c

HEADERS += \
    l3dt/l3dt.h \
    include/acknext/ext/terrain.h

RAWDATA += \
    resources/terrain.tesc \
    resources/terrain.tese \
    resources/terrain.frag \
    resources/terrain.vert

DISTFILES += \
    resources/terrain.tesc \
    resources/terrain.tese \
    resources/terrain.frag \
    resources/terrain.vert

