TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../acknext/release/ -lacknext
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../acknext/debug/ -lacknext
else:unix: LIBS += -L$$OUT_PWD/../acknext/ -lacknext

INCLUDEPATH += $$PWD/../acknext
DEPENDPATH += $$PWD/../acknext

INCLUDEPATH += $$PWD/../include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../tcc-0.9.26/release/ -ltcc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../tcc-0.9.26/debug/ -ltcc
else:unix: LIBS += -L$$PWD/../../tcc-0.9.26/ -ltcc

INCLUDEPATH += $$PWD/../../tcc-0.9.26
DEPENDPATH += $$PWD/../../tcc-0.9.26

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../tcc-0.9.26/release/libtcc.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../tcc-0.9.26/debug/libtcc.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../tcc-0.9.26/release/tcc.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../tcc-0.9.26/debug/tcc.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../tcc-0.9.26/libtcc.a

LIBS += -ldl
