TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += assimp

CONFIG += acknext-main acknext-default

include($$TOPDIR/acknext/acknext.pri)
include($$TOPDIR/addons/addons.pri)

SOURCES += main.c \
    loadmodel.cpp

DISTFILES += \
    ../scripts/bones.vert \
    ../scripts/bones.frag
