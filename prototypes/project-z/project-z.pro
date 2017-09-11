TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += zlib


include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

SOURCES += default.c \
    main.c

DISTFILES += \
    resources/shaders/terrain.vert \
    resources/shaders/terrain.tesc \
    resources/shaders/terrain.tese \
    resources/shaders/terrain.frag


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../addons/terrain/release/ -lterrain
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../addons/terrain/debug/ -lterrain
else:unix: LIBS += -L$$OUT_PWD/../../addons/terrain/ -lterrain

INCLUDEPATH += $$PWD/../../addons/terrain
DEPENDPATH += $$PWD/../../addons/terrain
