unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += ode dotconf sdl2 SDL2_image SDL2_mixer zlib

include(../../gl3w/gl3w.pri)

ACKBUILD=/tmp/build-acknext-Desktop-Debug/acknext

win32:CONFIG(release, debug|release): LIBS += -L$$ACKBUILD/release/ -lacknext
else:win32:CONFIG(debug, debug|release): LIBS += -L$$ACKBUILD/debug/ -lacknext
else:unix: LIBS += -L$$ACKBUILD -lacknext

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

INCLUDEPATH += $$PWD/include

message("acknext.pri PWD" $$PWD ", " $$ACKBUILD)
