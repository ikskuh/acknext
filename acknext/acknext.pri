unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += ode dotconf sdl2 SDL2_image SDL2_mixer zlib

include($$PWD/../extern/gl3w/gl3w.pri)

# TODO: Determine somehow...
ACKBUILD=/tmp/build-acknext-Desktop-Debug/acknext

win32:CONFIG(release, debug|release): LIBS += -L$$ACKBUILD/release/ -lacknext
else:win32:CONFIG(debug, debug|release): LIBS += -L$$ACKBUILD/debug/ -lacknext
else:unix: LIBS += -L$$ACKBUILD -lacknext

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD/include
