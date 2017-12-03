TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2 zlib

include($$TOPDIR/extern/gl3w/gl3w.pri)
unix: PKGCONFIG += assimp gl

SOURCES += main.c
