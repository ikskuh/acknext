TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += zlib


include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

SOURCES += \
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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../addons/ackmain/release/ -lackmain
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../addons/ackmain/debug/ -lackmain
else:unix: LIBS += -L$$OUT_PWD/../../addons/ackmain/ -lackmain

INCLUDEPATH += $$PWD/../../addons/ackmain
DEPENDPATH += $$PWD/../../addons/ackmain

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../addons/default/release/ -ldefault
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../addons/default/debug/ -ldefault
else:unix: LIBS += -L$$OUT_PWD/../../addons/default/ -ldefault

INCLUDEPATH += $$PWD/../../addons/default
DEPENDPATH += $$PWD/../../addons/default
