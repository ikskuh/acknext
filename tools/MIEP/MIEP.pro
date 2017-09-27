#-------------------------------------------------
#
# Project created by QtCreator 2017-08-12T12:01:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MIEP
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$PWD/../../common.pri)
include($$PWD/../../acknext/acknext.pri)

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += assimp

INCLUDEPATH += $$PWD/../acknext/include

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    qacknextwidget.cpp \
    materialeditor.cpp \
    ackcolorselector.cpp \
    imageview.cpp \
    boneeditor.cpp \
    bonetreemodel.cpp \
    meshlist.cpp \
    meshlistmodel.cpp \
    animationviewer.cpp \
    modelloader.cpp \
    modelsaver.cpp \
    bitmaskeditor.cpp \
    modelanimationlistmodel.cpp

HEADERS += \
        mainwindow.hpp \
    qacknextwidget.hpp \
    materialeditor.hpp \
    ackcolorselector.hpp \
    imageview.hpp \
    boneeditor.hpp \
    bonetreemodel.hpp \
    meshlist.hpp \
    meshlistmodel.hpp \
    animationviewer.hpp \
    modelloader.hpp \
    modelsaver.hpp \
    bitmaskeditor.hpp \
    modelanimationlistmodel.hpp

FORMS += \
        mainwindow.ui \
    materialeditor.ui \
    ackcolorselector.ui \
    boneeditor.ui \
    meshlist.ui \
    animationviewer.ui \
    bitmaskeditor.ui

RESOURCES += \
    ../../resource/ackui.qrc
