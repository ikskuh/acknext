QT       -= core gui

TARGET = ackcef
TEMPLATE = lib
# CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

include($$TOPDIR/acknext/acknext.pri)

LIBS += -L/opt/cef-minimal/Release/ -lcef
INCLUDEPATH += /opt/cef-minimal
INCLUDEPATH += /opt/cef-minimal/include
DEPENDPATH  += /opt/cef-minimal/include
LIBS += -L/opt/cef-minimal/libcef_dll_wrapper -lcef_dll_wrapper

SOURCES += \
    ackcef.cpp

HEADERS += \
	include/acknext/ext/ackcef.h
