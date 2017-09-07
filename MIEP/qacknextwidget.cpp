#include "qacknextwidget.hpp"

#include <QSurfaceFormat>
#include <QApplication>
#include <QMouseEvent>

QAcknextWidget::QAcknextWidget(QWidget *parent) : QOpenGLWidget(parent), mModelDisplay(nullptr)
{
	QSurfaceFormat format;
	format.setVersion(4, 5);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setOption(QSurfaceFormat::DebugContext, true);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setSwapInterval(1); // Vsync
	this->setFormat(format);
}

QAcknextWidget::~QAcknextWidget()
{
	ent_remove(this->mModelDisplay);
	// view_remove(this->mView);
	// engine_close();
}

void QAcknextWidget::initializeGL()
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

	LIGHT * ambi = light_create(AMBIENTLIGHT);
	ambi->color = (COLOR){0.3,0.3,0.3,1.0};

	camera->position.z = 64;


	this->mModelDisplay = ent_create(nullptr, vector(0,0,0), NULL);
}

void QAcknextWidget::setModel(MODEL *model)
{
	this->mModelDisplay->model = model;
	this->update();
}

void QAcknextWidget::resizeGL(int w, int h)
{
	engine_resize(w, h);
	this->update();
}

void QAcknextWidget::paintGL()
{
	engine_frame();

	const_cast<SIZE&>(screen_size) = (SIZE) { this->width(), this->height() };
	render_scene_with_camera(camera);
}

void QAcknextWidget::mousePressEvent(QMouseEvent *event)
{
	this->mPrevPos = event->pos();
}

void QAcknextWidget::mouseMoveEvent(QMouseEvent *event)
{
	auto delta = event->pos() - this->mPrevPos;
	this->mPrevPos = event->pos();

	QUATERNION qx, qy;
	quat_axis_angle(&qx, vector(0, 1, 0), delta.x());
	quat_axis_angle(&qy, vector(1, 0, 0), delta.y());

	quat_mult(&qx, &qy);
	quat_mult(&qx, &this->mModelDisplay->rotation);

	this->mModelDisplay->rotation = qx;

	this->update();
}

void QAcknextWidget::wheelEvent(QWheelEvent *event)
{
	camera->position.z -= event->delta() / 120;
	this->update();
}
