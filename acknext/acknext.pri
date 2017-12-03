unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += ode dotconf sdl2 SDL2_image SDL2_mixer zlib

# Include gl3w as header-only (symbols are included in libacknext)
INCLUDEPATH += $$TOPDIR/extern/gl3w/include
DEPENDPATH  += $$TOPDIR/extern/gl3w/include

LIBS += -lacknext

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD/include
