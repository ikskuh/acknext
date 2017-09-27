#include "mainwindow.hpp"
#include <QApplication>
#include <QCoreApplication>

#include <acknext.h>

int main(int argc, char *argv[])
{
	engine_config.argv0 = argv[0];
	engine_config.flags |= CUSTOM_VIDEO;
	engine_config.flags &= ~USE_VFS;
	engine_config.flags |= SILENT_FAIL;
	engine_config.application = "Team Retro";
	engine_config.application = "AckMIEP";

	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
