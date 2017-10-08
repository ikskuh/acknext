# Provide config options for each addon

acknext-configfile {
	DEFINES += ACKNEXT_HAS_EXT_CONFIGFILE
	LIBS += -L$$OUT_TOPDIR/addons/configfile/ -lconfigfile

	INCLUDEPATH += $$PWD/configfile/include
	DEPENDPATH += $$PWD/configfile/include
}

acknext-scheduler {
	DEFINES += ACKNEXT_HAS_EXT_SCHEDULER
	LIBS += -L$$OUT_TOPDIR/addons/scheduler/ -lscheduler

	INCLUDEPATH += $$PWD/scheduler/include
	DEPENDPATH += $$PWD/scheduler/include
}

acknext-terrain {
	DEFINES += ACKNEXT_HAS_EXT_TERRAIN
	LIBS += -L$$OUT_TOPDIR/addons/terrain/ -lterrain

	INCLUDEPATH += $$PWD/terrain/include
	DEPENDPATH += $$PWD/terrain/include
}

acknext-imgui {
	DEFINES += ACKNEXT_HAS_EXT_IMGUI
	LIBS += -L$$OUT_TOPDIR/addons/dearackgui/ -ldearackgui

	INCLUDEPATH += $$PWD/dearackgui/include
	INCLUDEPATH += $$TOPDIR/extern/imgui/include
	DEPENDPATH  += $$PWD/dearackgui
}

acknext-default {
	DEFINES += ACKNEXT_HAS_EXT_DEFAULT
	LIBS += -L$$OUT_TOPDIR/addons/default/ -ldefault

	INCLUDEPATH += $$PWD/default/include
	DEPENDPATH += $$PWD/default/include
}

acknext-main {
	DEFINES += ACKNEXT_HAS_EXT_MAIN
	LIBS += -L$$OUT_TOPDIR/addons/ackmain/ -lackmain
}

acknext-cef {
	DEFINES += ACKNEXT_HAS_EXT_CEF
	LIBS += -L$$OUT_TOPDIR/addons/ackcef/ -lackcef

	INCLUDEPATH += $$PWD/ackcef/include
	DEPENDPATH += $$PWD/ackcef/include

	# TODO: Somehow include this in the repo(?)
	LIBS += -L/opt/cef-minimal/Release/ -lcef
	LIBS += -L/opt/cef-minimal/libcef_dll_wrapper -lcef_dll_wrapper
}
