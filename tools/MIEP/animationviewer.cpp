#include "animationviewer.hpp"
#include "ui_animationviewer.h"

#include <float.h>
#include <math.h>

#include <QListView>
#include <QLineEdit>
#include <QSlider>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QCheckBox>

AnimationViewer::AnimationViewer(MODEL * model, QWidget *parent) :
    QDockWidget(parent),
    model(model),
    listModel(new ModelAnimationListModel(model)),
    timer(new QTimer()),
    ui(new Ui::AnimationViewer)
{
	ui->setupUi(this);

	ui->list->setModel(this->listModel);

	connect(
		ui->list->selectionModel(),
		&QItemSelectionModel::currentChanged,
		this,
		&AnimationViewer::on_list_currentChanged);

	this->selectAnim(nullptr);

	connect(
		this->timer, &QTimer::timeout,
		this, &AnimationViewer::animateFurther);
	this->timer->setSingleShot(false);
	this->timer->setInterval(25);
}

AnimationViewer::~AnimationViewer()
{
	delete ui;
	delete listModel;
	delete timer;
}

void AnimationViewer::selectAnim(ANIMATION *anim)
{
	bool enabled = (anim != nullptr);

	this->selection = anim;

	ui->name->setEnabled(enabled);
	ui->play->setEnabled(enabled);
	ui->rewind->setEnabled(enabled);
	ui->stop->setEnabled(enabled);
	ui->remove->setEnabled(enabled);
	ui->position->setEnabled(enabled);
	ui->progress->setEnabled(enabled);
	ui->looped->setEnabled(enabled);

	if(enabled) {
		this->timer->stop();
		ui->name->setText(QString(anim->name));
		ui->looped->setChecked(anim->flags & LOOPED);
		ui->progress->setValue(0);
		this->animate(0);
	}
}

void AnimationViewer::animateFurther()
{
	assert(this->selection);
	this->progress += 0.001 * this->timer->interval();

	if(this->progress >= this->selection->duration) {
		if(this->selection->flags & LOOPED) {
			this->progress -= this->selection->duration;
		} else {
			this->progress = this->selection->duration;
			this->timer->stop();
		}
	}

	ui->progress->setValue(100 * this->progress / selection->duration);

	this->animate(this->progress);
}

void AnimationViewer::animate(double frameTime)
{
	assert(this->model);
	assert(this->selection);
	auto * anim = this->selection;

	this->progress = frameTime;

	for(int i = 0; i < anim->channelCount; i++)
	{
		CHANNEL const * chan = anim->channels[i];
		try {
			BONE & bone = model->bones[chan->targetBone];
			assert(chan->frameCount > 0);

			FRAME frame = chan->frames[0];
			for(int j = 1; j < chan->frameCount; j++) {
				if(chan->frames[j].time > frameTime) break;
				frame = chan->frames[j];
			}

			MATRIX transform;
			mat_id(&transform);
			mat_scale(&transform, &frame.scale);
			mat_rotate(&transform, &frame.rotation);
			mat_translate(&transform, &frame.position);

			bone.transform = transform;
		} catch(std::exception & ex) {
			engine_log("Failed to animate bone %d: %s", chan->targetBone, ex.what());
		}
	}

	engine_log("Animate %s @ %f / %f", anim->name, frameTime, anim->duration);

	double dur = this->selection->duration;
	double pos = this->progress;

	this->ui->position->setText(QString("%1 / %2")
		.arg(pos,4,'f',2)
		.arg(dur,4,'f',2));

	emit this->hasChanged();
}

void AnimationViewer::on_list_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	Q_UNUSED(previous);
	this->selectAnim(this->listModel->get(current));
}

void AnimationViewer::on_list_doubleClicked(const QModelIndex &index)
{
	this->selectAnim(this->listModel->get(index));
	this->on_play_clicked();
}

void AnimationViewer::on_progress_valueChanged(int value)
{
	Q_UNUSED(value);
	if(this->timer->isActive() == false) {
		this->animate(this->selection->duration * value / 100.0);
	}
}

void AnimationViewer::on_play_clicked()
{
	this->timer->start();
}

void AnimationViewer::on_stop_clicked()
{
	this->timer->stop();
}

void AnimationViewer::on_rewind_clicked()
{
	this->timer->stop();
    this->progress = 0;
	ui->progress->setValue(0);
}

void AnimationViewer::on_looped_clicked(bool checked)
{
    assert(this->selection);
	this->selection->flags &= ~LOOPED;
	if(checked) {
		this->selection->flags |= LOOPED;
	}
}
