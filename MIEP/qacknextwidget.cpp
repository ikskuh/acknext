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

	this->mModelDisplay = ent_create(nullptr, vector(0,0,0), NULL);
}

QAcknextWidget::~QAcknextWidget()
{
	ent_remove(this->mModelDisplay);
	// view_remove(this->mView);
	// engine_close();
}

void QAcknextWidget::initializeGL()
{
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
