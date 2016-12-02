TEMPLATE = lib
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    engine.cpp

HEADERS += \
    $$PWD/../include/acknext.h \
    $$PWD/../include/acktypes.h \
    ../include/ackcfg.h

INCLUDEPATH += $$PWD/../include/

DEFINES += _ACKNEXT_INTERNAL_
