TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    acknext \
	demo-c \
	shadertest \
    mtlconv \
    MIEP \
    project-z

HEADERS += \
    documentation/src/groups.h \
    documentation/src/mainpage.h

DISTFILES += \
    resource/shaders/lighting.glsl \
    resource/shaders/gamma.glsl \
    resource/shaders/acklights.glsl

SOURCES += \
    l3dt/heightfield.c
