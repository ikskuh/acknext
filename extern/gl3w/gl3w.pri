INCLUDEPATH += $$PWD/include
DEPENDPATH  += $$PWD/include

SOURCES += $$PWD/src/gl3w.c
HEADERS += \
	$$PWD/include/GL/gl3w.h \
	$$PWD/include/GL/glcorearb.h

LIBS += -ldl -lGL
