TEMPLATE = lib
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    engine.cpp \
    variables.cpp \
    renderer.cpp \
    scheduler.cpp \
    $$PWD/../coroutine/coroutine.c

HEADERS += \
    $$PWD/../include/acknext.h \
    $$PWD/../include/acktypes.h \
    $$PWD/../include/ackvars.h \
    ../include/ackcfg.h \
    engine.h \
    $$PWD/../coroutine/coroutine.h \
    ../include/ackcpp.h

INCLUDEPATH += $$PWD/../include/

DEFINES += _ACKNEXT_INTERNAL_


unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2 SDL2_image SDL2_mixer gl
