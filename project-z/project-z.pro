TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += zlib

include(../../gl3w/gl3w.pri)

QMAKE_CFLAGS   += -g -rdynamic
QMAKE_CXXFLAGS += -g -rdynamic
QMAKE_LFLAGS   += -g -rdynamic

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../acknext/release/ -lacknext
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../acknext/debug/ -lacknext
else:unix: LIBS += -L$$OUT_PWD/../acknext/ -lacknext

INCLUDEPATH += $$PWD/../acknext
DEPENDPATH += $$PWD/../acknext

INCLUDEPATH += $$PWD/../acknext/include

SOURCES += default.c \
    main.c \
    ../l3dt/heightfield.c \
    ../l3dt/attributefield.c

DISTFILES += \
    resources/shaders/terrain.vert \
    resources/shaders/terrain.tesc \
    resources/shaders/terrain.tese \
    resources/shaders/terrain.frag

HEADERS += \
    ../l3dt/l3dt.h

