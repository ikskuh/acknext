TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    default.c

include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

# LIBS += -L$$PWD/../../tcc-0.9.26/ -ldl -ltcc
# DEPENDPATH += $$PWD/../../tcc-0.9.26
