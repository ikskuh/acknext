TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c

CONFIG += acknext-main acknext-default

include($$TOPDIR/acknext/acknext.pri)
include($$TOPDIR/addons/addons.pri)
