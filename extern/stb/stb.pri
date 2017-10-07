INCLUDEPATH += $$PWD/include
DEPENDPATH  += $$PWD/include

HEADERS += \
	$$PWD/include/stb.h \
	$$PWD/include/stb_connected_components.h \
	$$PWD/include/stb_c_lexer.h \
	$$PWD/include/stb_divide.h \
	$$PWD/include/stb_dxt.h \
	$$PWD/include/stb_easy_font.h \
	$$PWD/include/stb_herringbone_wang_tile.h \
	$$PWD/include/stb_image.h \
	$$PWD/include/stb_image_resize.h \
	$$PWD/include/stb_image_write.h \
	$$PWD/include/stb_leakcheck.h \
	$$PWD/include/stb_perlin.h \
	$$PWD/include/stb_rect_pack.h \
	$$PWD/include/stb_sprintf.h \
	$$PWD/include/stb_textedit.h \
	$$PWD/include/stb_tilemap_editor.h \
	$$PWD/include/stb_truetype.h \
	$$PWD/include/stb_voxel_render.h \
	$$PWD/include/stretchy_buffer.h

stb_vorbis {
	SOURCES += $$PWD/src/stb_vorbis.c
}