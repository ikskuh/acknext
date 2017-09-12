QT       -= core gui

TARGET = ackcef
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

LIBS += -L/opt/cef-minimal/Release/ -lcef
INCLUDEPATH += /opt/cef-minimal
INCLUDEPATH += /opt/cef-minimal/include
DEPENDPATH  += /opt/cef-minimal/include

SOURCES += \
    ackcef.cpp

HEADERS += \
        ackcef.h
