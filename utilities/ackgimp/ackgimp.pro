#-------------------------------------------------
#
# Project created by QtCreator 2017-09-30T18:37:40
#
#-------------------------------------------------

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

# this is neat :)
DESTDIR = $$system(echo ~)/.gimp-2.8/plug-ins/

DEFINES += ACKGIMP_LIBRARY

SOURCES += \
        ackgimp.c

HEADERS +=

LIBS += -lm

QMAKE_CFLAGS += `gimptool-2.0 --cflags`
QMAKE_CXXFLAGS += `gimptool-2.0 --cflags`
QMAKE_LFLAGS += `gimptool-2.0 --libs`
