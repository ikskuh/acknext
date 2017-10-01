#-------------------------------------------------
#
# Project created by QtCreator 2017-09-30T18:37:40
#
#-------------------------------------------------

QT       -= core gui

CONFIG -= qt

TARGET = ackgimp
TEMPLATE = lib

DEFINES += ACKGIMP_LIBRARY

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gtk+-2.0 gimp-2.0 glib-2.0 gimp-2.0 gimpui-2.0

SOURCES += \
        ackgimp.c

HEADERS +=

INCLUDEPATH += \
	/usr/include/gimp-2.0 \
	/usr/include/gdk-pixbuf-2.0 \
	/usr/include/cairo \
	/usr/include/pixman-1 \
	/usr/include/freetype2 \
	/usr/include/libpng16 \
	/usr/include/harfbuzz \
	/usr/include/glib-2.0 \
	/usr/lib/glib-2.0/include \
	/usr/include/freetype2 \
	/usr/include/harfbuzz \
	/usr/include/glib-2.0 \
	/usr/lib/glib-2.0/include \
	/usr/include/libpng16 \
	/usr/include/gtk-2.0 \
	/usr/lib/gtk-2.0/include \
	/usr/include/pango-1.0 \
	/usr/include/atk-1.0 \
	/usr/include/cairo \
	/usr/include/pixman-1 \
	/usr/include/gdk-pixbuf-2.0 \
	/usr/include/pango-1.0 \
	/usr/include/freetype2 \
	/usr/include/libpng16 \
	/usr/include/harfbuzz \
	/usr/include/glib-2.0 \
	/usr/lib/glib-2.0/include \
	/usr/include/freetype2 \
	/usr/include/libpng16 \
	/usr/include/harfbuzz \
	/usr/include/glib-2.0 \
	/usr/lib/glib-2.0/include
