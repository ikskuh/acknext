TEMPLATE = lib
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG    += link_pkgconfig
PKGCONFIG += assimp ode dotconf

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
    buffer.cpp \
    bitmap.cpp \
    material.cpp \
    entity.cpp \
    matrix.cpp \
    utils.cpp \
    engineconfig.cpp \
    collision.cpp

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
    ackglm.hpp \
    buffer-detail.h \
    bitmap-detail.h \
    shader-detail.h \
    entity-detail.h \
    engineconfig.h

INCLUDEPATH += $$PWD/../include/

DEFINES += _ACKNEXT_INTERNAL_
DEFINES += "ACKNEXT_LEVEL_DETAIL='struct LEVELdetail'"
DEFINES += "ACKNEXT_BITMAP_DETAIL='struct BITMAPdetail'"
DEFINES += "ACKNEXT_ENTITY_DETAIL='struct ENTITYdetail'"


# QMAKE_LINK=ld
# message($$QMAKE_LINK)

# QMAKE_LFLAGS += -rpath $$PWD/../../tcc-0.9.26/ -ltcc

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sdl2 SDL2_image SDL2_mixer gl

LIBS += -L$$PWD/../../tcc-0.9.26/ -ldl -ltcc

INCLUDEPATH += $$PWD/../../tcc-0.9.26
DEPENDPATH += $$PWD/../../tcc-0.9.26

# Used for levels
INCLUDEPATH += $$PWD/../../json/src

DISTFILES += \
    linker.ld

QMAKE_LFLAGS += -T$$quote($$PWD/linker.ld)

include(../../gl3w/gl3w.pri)
