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
	view_remove(this->mView);
	engine_close();
}

void QAcknextWidget::initializeGL()
{
	engine_config.resolution = (SIZE) {
		this->width(),
		this->height()
	};
	engine_open();

	LIGHT * sun = light_create(SUNLIGHT);
	sun->direction = (VECTOR){ 0, -1, 0 };
	// vec_normalize(&sun->direction, 1.0);

	LIGHT * ambi = light_create(AMBIENTLIGHT);
	ambi->color = (COLOR){0.3,0.3,0.3,1.0};

	camera->position.z = 16;

	this->mView = view_create((RENDERCALL)render_scene_with_camera, camera);

	this->mModelDisplay = ent_create(nullptr, vector(0,0,0), NULL);
}

void QAcknextWidget::setModel(MODEL *model)
{
	this->mModelDisplay->model = model;
	if(model != nullptr) {
		camera->position.z = 1.2 * maxv(model->boundingBox.maximum.z, 1);
		ent_posereset(this->mModelDisplay);
	}
	this->update();
}

void QAcknextWidget::resizeGL(int w, int h)
{
	engine_resize(w, h);
	this->update();
}

void QAcknextWidget::drawBones()
{
	MATRIX mat;
	mat_id(&mat);

	MATRIX animatedBones[ACKNEXT_MAX_BONES];
	for(int i = 0; i < ACKNEXT_MAX_BONES; i++)
	{
		MATRIX & transform = animatedBones[i];
		mat_id(&transform);
		mat_translate(&transform, &this->mModelDisplay->pose[i].position);
		mat_rotate(&transform, &this->mModelDisplay->pose[i].rotation);
		mat_scale(&transform, &this->mModelDisplay->pose[i].scale);
	}

	MATRIX transforms[ACKNEXT_MAX_BONES];
	VECTOR positions[ACKNEXT_MAX_BONES];
	transforms[0] = animatedBones[0];

	positions[0] = nullvector;
	vec_transform(&positions[0], &transforms[0]);

	for(int i = 1; i < model()->boneCount; i++)
	{
		BONE * bone = &model()->bones[i];
		mat_mul(&transforms[i], &animatedBones[i], &transforms[bone->parent]);

		positions[i] = nullvector;
		vec_transform(&positions[i], &transforms[i]);

		draw_line3d(&positions[i], &positions[bone->parent], &COLOR_GREEN);

		// if(i == selection) {
		// 	draw_point3d(&positions[i], &COLOR_RED);
		// } else {
			draw_point3d(&positions[i], &COLOR_BLUE);
		// }
	}
}

void QAcknextWidget::paintGL()
{
	draw_line3d(vector(0,0,0), vector(100,0,0), &COLOR_RED);
	draw_line3d(vector(0,0,0), vector(0,100,0), &COLOR_GREEN);
	draw_line3d(vector(0,0,0), vector(0,0,100), &COLOR_BLUE);

	if(this->mShowSkeleton && this->model()) {
		this->drawBones();
	}

	if(this->mShowNormals)
	{
		MODEL * mod = mModelDisplay->model;
		if(mod) {
			for(int i = 0; i < mod->meshCount; i++) {
				MESH * mesh = mod->meshes[i];

				int vertexCount = mesh->vertexBuffer->size / sizeof(VERTEX);
				VERTEX * vertices = (VERTEX*)buffer_map(mesh->vertexBuffer, READONLY);

				for(int i = 0; i < vertexCount; i++) {
					draw_line3d(
						&vertices[i].position,
						vec_add(vec_clone(&vertices[i].position), &vertices[i].normal),
						&COLOR_CYAN);
				}
				buffer_unmap(mesh->vertexBuffer);
			}
		}
	}
	engine_frame();
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
	quat_mult(&camera->rotation, &qx);

	// camera->rotation = qx;

	var dist = vec_length(&camera->position);

	vec_set(&camera->position, vector(0, 0, dist));
	vec_rotate(&camera->position, &camera->rotation);

	// this->mModelDisplay->rotation = qx;

	this->update();
}

void QAcknextWidget::wheelEvent(QWheelEvent *event)
{
	vec_normalize(&camera->position, maxv(1, vec_length(&camera->position) + event->delta() / 120));
	this->update();
}
