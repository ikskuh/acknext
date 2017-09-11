#include "animationviewer.hpp"
#include "ui_animationviewer.h"

AnimationViewer::AnimationViewer(MODEL * model, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::AnimationViewer)
{
	ui->setupUi(this);
}

AnimationViewer::~AnimationViewer()
{
	delete ui;
}
