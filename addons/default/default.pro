TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
# CONFIG += staticlib

include($$TOPDIR/acknext/acknext.pri)

SOURCES += \
    default.c

HEADERS += \
    include/acknext/ext/default.h
