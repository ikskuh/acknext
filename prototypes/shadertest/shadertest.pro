TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += assimp

SOURCES += main.c \
    default.c \
    loadmodel.cpp

include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)


# LIBS += -L$$PWD/../../tcc-0.9.26/ -ldl -ltcc
# DEPENDPATH += $$PWD/../../tcc-0.9.26

DISTFILES += \
    ../scripts/bones.vert \
    ../scripts/bones.frag
