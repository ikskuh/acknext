TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += zlib

CONFIG += acknext-terrain acknext-imgui acknext-default

include($$TOPDIR/acknext/acknext.pri)
include($$TOPDIR/addons/addons.pri)

SOURCES += \
    main.cpp \
    gui_drawperf.cpp

DISTFILES += \
    resources/shaders/fastobject.frag \
    resources/shaders/fastobject.vert
