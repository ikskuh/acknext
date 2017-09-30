QMAKE_CFLAGS   += -g -rdynamic -O3
QMAKE_CXXFLAGS += -g -rdynamic -O3
QMAKE_LFLAGS   += -g -rdynamic -O3

DISTFILES += \
    $$PWD/resource/shaders/postprocess.vert \
    $$PWD/resource/shaders/pp/gammacorrect.frag \
    $$PWD/resource/shaders/pp/hdr/linear.frag
