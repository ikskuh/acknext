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

AnimationViewer::AnimationViewer(MODEL * model, QWidget *parent) :
    QDockWidget(parent),
    model(model),
    listModel(new ModelAnimationListModel(model)),
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
}

AnimationViewer::~AnimationViewer()
{
	delete ui;
	delete listModel;
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

	if(enabled) {
		ui->name->setText(QString(anim->name));

		ui->progress->setValue(0);

		this->updatePosition();
	}
}

void AnimationViewer::updatePosition()
{
	double dur = this->selection->duration;
	double pos = dur * ui->progress->value() / 100.0;

	this->ui->position->setText(QString("%1 / %2").arg(pos,3).arg(dur,3));

	this->animate(pos);

	emit this->hasChanged();
}


void AnimationViewer::animate(double frameTime)
{
	assert(this->model);
	assert(this->selection);
	auto * anim = this->selection;

	std::map<std::string, BONE&> bones;
	for(int i = 0; i < model->boneCount; i++) {
		bones.emplace(std::string(model->bones[i].name), model->bones[i]);
	}

	/*
	if(anim->mTicksPerSecond > 0) {
		frameTime *= anim->mTicksPerSecond;
	} else {
		frameTime *= 25.0;
	}
	*/

	if(anim->duration > 0) {
		frameTime = fmod(frameTime, anim->duration);
	}

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
}

void AnimationViewer::on_list_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	Q_UNUSED(previous);
	this->selectAnim(this->listModel->get(current));
}

void AnimationViewer::on_progress_valueChanged(int value)
{
    this->updatePosition();
}
