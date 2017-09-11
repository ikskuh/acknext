#include "modelsaver.hpp"
#include "mainwindow.hpp"

#include <acknext/serialization.h>
static const uint32_t ACKNEXT_MAGIC = 0xCC41E367;

static const ACKGUID guid = {{
	0xc4, 0xa6, 0x7f, 0xe0,
	0x82, 0x74, 0x43, 0x90,
	0x8e, 0xd6, 0x50, 0x61,
	0x0b, 0x0a, 0x54, 0xf8
}};

void ModelSaver::write(ACKFILE * file, MODEL const * model)
{
	MainWindow::makeCurrent();
	model_write(file, model);
}
