TEMPLATE = lib
CONFIG += c++11
# CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

SOURCES += \
    ../../../imgui/imgui.cpp \
    ../../../imgui/imgui_draw.cpp \
    ../../../imgui/imgui_demo.cpp \
    ackgui.cpp

HEADERS += \
    ackgui.h

INCLUDEPATH += $$PWD/../../../imgui/
