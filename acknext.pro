TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    acknext \
    tools \
	prototypes \
    addons

HEADERS += \
    documentation/src/groups.h \
    documentation/src/mainpage.h

DISTFILES += \
    resource/shaders/lighting.glsl \
    resource/shaders/gamma.glsl \
    resource/shaders/acklights.glsl \
    resource/shaders/fog.glsl

