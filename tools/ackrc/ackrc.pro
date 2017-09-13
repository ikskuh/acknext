TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    process-material.cpp \
    process-texture.cpp \
    main.cpp

include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

HEADERS += \
    json.hpp
