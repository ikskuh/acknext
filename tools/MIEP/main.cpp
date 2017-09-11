#include "mainwindow.hpp"
#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
