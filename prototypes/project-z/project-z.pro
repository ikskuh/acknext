TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += zlib


include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

SOURCES += \
    main.cpp

DISTFILES += \
    resources/shaders/terrain.vert \
    resources/shaders/terrain.tesc \
    resources/shaders/terrain.tese \
    resources/shaders/terrain.frag \
    resources/shaders/fastobject.frag \
    resources/shaders/fastobject.vert


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../addons/terrain/release/ -lterrain
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../addons/terrain/debug/ -lterrain
else:unix: LIBS += -L$$OUT_PWD/../../addons/terrain/ -lterrain

INCLUDEPATH += $$PWD/../../addons/terrain/include
DEPENDPATH += $$PWD/../../addons/terrain/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../addons/default/release/ -ldefault
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../addons/default/debug/ -ldefault
else:unix: LIBS += -L$$OUT_PWD/../../addons/default/ -ldefault

INCLUDEPATH += $$PWD/../../addons/default
DEPENDPATH += $$PWD/../../addons/default

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../addons/ackcef/release/ -lackcef
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../addons/ackcef/debug/ -lackcef
else:unix: LIBS += -L$$OUT_PWD/../../addons/ackcef/ -lackcef

INCLUDEPATH += $$PWD/../../addons/ackcef
DEPENDPATH += $$PWD/../../addons/ackcef
LIBS += -L/opt/cef-minimal/Release/ -lcef
LIBS += -L/opt/cef-minimal/libcef_dll_wrapper -lcef_dll_wrapper


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../addons/dearackgui/release/ -ldearackgui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../addons/dearackgui/debug/ -ldearackgui
else:unix: LIBS += -L$$OUT_PWD/../../addons/dearackgui/ -ldearackgui

INCLUDEPATH += $$PWD/../../addons/dearackgui
INCLUDEPATH += $$PWD/../../../imgui
DEPENDPATH += $$PWD/../../addons/dearackgui


#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../addons/ackcef/release/libackcef.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../addons/ackcef/debug/libackcef.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../addons/ackcef/release/ackcef.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../addons/ackcef/debug/ackcef.lib
#else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../addons/ackcef/libackcef.a
