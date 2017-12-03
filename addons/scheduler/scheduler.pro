TEMPLATE = lib
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

include($$TOPDIR/acknext/acknext.pri)
include($$TOPDIR/extern/coroutine/coroutine.pri)

DISTFILES += \
	linker.ld
QMAKE_LFLAGS += -T$$quote($$PWD/linker.ld)

DEFINES += _ACKNEXT_EXT_SCHEDULER_INTERNAL_

SOURCES += \
    src/scheduler.cpp

HEADERS +=
