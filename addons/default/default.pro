TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
CONFIG += staticlib

include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

SOURCES += \
    default.c

HEADERS += \
    default.h
