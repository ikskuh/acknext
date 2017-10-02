TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)
