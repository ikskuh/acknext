TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2 SDL2_image

SOURCES += main.c
