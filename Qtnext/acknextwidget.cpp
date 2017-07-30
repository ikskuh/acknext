#include "acknextwidget.h"
#include <acknext.h>
#include <QTimer>
#include <QBasicTimer>
#include <QDebug>

#undef start

AcknextWidget::AcknextWidget(QWidget *parent) : QOpenGLWidget(parent)
{
	QSurfaceFormat fmt;
	fmt.setVersion(4, 5);
	this->setFormat(fmt);
}

void AcknextWidget::initializeGL()
{
	static AcknextWidget * widg = nullptr;
	Q_ASSERT(!widg);
	widg = this;
	engine_backend.swapBuffers = [](){};
	engine_backend.getSize = [](int*w,int*h){*w = widg->width(); *h = widg->height(); };
	char * argv[] =
	{
	    "Qtnext",
		"--no-sdl",
	    "main.c"
	};
	engine_open(3, argv);
}

void AcknextWidget::paintGL()
{
	engine_frame();
	this->update();
}
