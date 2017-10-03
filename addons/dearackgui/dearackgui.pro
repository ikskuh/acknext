TEMPLATE = lib
CONFIG += c++11
# CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

include($$TOPDIR/acknext/acknext.pri)
include($$TOPDIR/extern/imgui/imgui.pri)

SOURCES += \
    ackgui.cpp

HEADERS += \
    include/acknext/ext/ackgui.h
