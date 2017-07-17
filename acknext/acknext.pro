TEMPLATE = lib
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    engine.cpp \
    variables.cpp \
    renderer.cpp \
    scheduler.cpp \
    compiler.cpp \
    $$PWD/../coroutine/coroutine.c \
    video.cpp \
    scene.cpp \
    input.cpp \
    view.cpp \
    model.cpp \
    shader.cpp \
    buffer.cpp

HEADERS += \
    $$PWD/../include/acknext.h \
    $$PWD/../include/acktypes.h \
    $$PWD/../include/ackvars.h \
    ../include/ackcfg.h \
    engine.h \
    $$PWD/../coroutine/coroutine.h \
    ../include/ackcpp.h \
    ../include/ackkeys.h \
    level-detail.h \
    model.h \
    ackglm.hpp

INCLUDEPATH += $$PWD/../include/

DEFINES += _ACKNEXT_INTERNAL_


# QMAKE_LINK=ld
# message($$QMAKE_LINK)

# QMAKE_LFLAGS += -rpath $$PWD/../../tcc-0.9.26/ -ltcc

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2 SDL2_image SDL2_mixer gl

LIBS += -L$$PWD/../../tcc-0.9.26/ -ldl -ltcc

INCLUDEPATH += $$PWD/../../tcc-0.9.26
DEPENDPATH += $$PWD/../../tcc-0.9.26

INCLUDEPATH += $$PWD/../../json/src

DISTFILES += \
    linker.ld

QMAKE_LFLAGS += -T$$quote($$PWD/linker.ld)

include(../../gl3w/gl3w.pri)
