TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    acknext \
    demo-cpp \
    demo-c

SOURCES += \
    scripts/main.c

HEADERS += \
    documentation/src/groups.h
