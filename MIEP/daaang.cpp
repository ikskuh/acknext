#include "daaang.hpp"

#include <QCoreApplication>
#include <QDebug>

Daaang::Daaang(QWidget *parent) : QOpenGLWidget(parent)
{

	QSurfaceFormat format;
	format.setVersion(4, 5);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setOption(QSurfaceFormat::DebugContext, true);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setSwapInterval(1); // Vsync
	this->setFormat(format);

	this->setMinimumSize(100, 100);
}

void Daaang::initializeGL()
{
	auto string = QCoreApplication::applicationFilePath().toUtf8();
	char * argv[] = {
		string.data(),
		"--no-sdl",
	};

	engine_open(2, argv);

	engine_resize(this->width(), this->height());

	filesys_addResource("/", "/");

	filesys_addResource("/home/felix/projects/acknext/scripts", "/demo");

	LIGHT * sun = light_create(SUNLIGHT);
	sun->direction = (VECTOR){ -10, 30, 20 };
	vec_normalize(&sun->direction, 1.0);


	camera->position.z = 64;

	emit done();
}
