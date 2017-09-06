#include "ackcolorselector.hpp"
#include "ui_ackcolorselector.h"

AckColorSelector::AckColorSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AckColorSelector)
{
	ui->setupUi(this);
}

AckColorSelector::~AckColorSelector()
{
	delete ui;
}
