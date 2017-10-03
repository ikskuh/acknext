# Compiles distfiles into header files

HEADERS += \
	$$PWD/resource-compiler.h

INCLUDEPATH += $$PWD

raw_rcc.CONFIG   = no_dependencies
raw_rcc.output   = ${QMAKE_FILE_IN_BASE}${QMAKE_FILE_IN_EXT}.o
raw_rcc.commands = \ # use local paths instead of absolute because of objcopy stupidity in symbol creation
	cd ${QMAKE_FILE_IN_PATH} && objcopy -B i386:x86-64 -I binary -O elf64-x86-64 ${QMAKE_FILE_IN_BASE}${QMAKE_FILE_IN_EXT} $$OUT_PWD/${QMAKE_FILE_OUT}
raw_rcc.input    = RAWDATA
raw_rcc.name     = Building embedded resource files

QMAKE_EXTRA_COMPILERS += raw_rcc
