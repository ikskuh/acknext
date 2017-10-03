TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    process-material.cpp \
    process-texture.cpp \
    main.cpp

include($$TOPDIR/acknext/acknext.pri)
include($$TOPDIR/extern/json/json.pri)
