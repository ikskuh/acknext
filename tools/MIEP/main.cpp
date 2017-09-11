#include "mainwindow.hpp"
#include <QApplication>
#include <QCoreApplication>

#include <acknext.h>

int main(int argc, char *argv[])
{
	engine_config.argv0 = argv[0];

	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
