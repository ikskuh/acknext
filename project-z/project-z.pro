TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include(../../gl3w/gl3w.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../acknext/release/ -lacknext
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../acknext/debug/ -lacknext
else:unix: LIBS += -L$$OUT_PWD/../acknext/ -lacknext

INCLUDEPATH += $$PWD/../acknext
DEPENDPATH += $$PWD/../acknext

INCLUDEPATH += $$PWD/../acknext/include

SOURCES += main.cpp default.c

DISTFILES += \
    resources/shaders/terrain.frag \
    resources/shaders/terrain.vert
