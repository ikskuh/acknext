TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c

include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

# LIBS += -L$$PWD/../../tcc-0.9.26/ -ldl -ltcc
# DEPENDPATH += $$PWD/../../tcc-0.9.26

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
