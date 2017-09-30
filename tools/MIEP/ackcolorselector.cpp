#include "ackcolorselector.hpp"
#include "ui_ackcolorselector.h"

#include <QPalette>
#include <QColorDialog>
#include <QFrame>
#include <QLabel>
#include <QSlider>

AckColorSelector::AckColorSelector(QWidget *parent) :
    QWidget(parent),
    mColor(QColor::fromRgbF(1.0, 1.0, 1.0)), mOverdrive(1.0),
    ui(new Ui::AckColorSelector)
{
	ui->setupUi(this);
}

AckColorSelector::~AckColorSelector()
{
	delete ui;
}

void AckColorSelector::setColor(COLOR color)
{
	this->mOverdrive = qMax(1.0f, qMax(color.red, qMax(color.green, color.blue)));
	this->mColor = QColor::fromRgbF(
		color.red / this->mOverdrive,
		color.green / this->mOverdrive,
		color.blue / this->mOverdrive);

	ui->driver->setValue(int(this->mOverdrive * 10));

	this->updateDisplay();

	emit colorChanged(color);
}

void AckColorSelector::updateDisplay()
{
	ui->overdrive->setText(tr("× %1").arg(this->mOverdrive, 0, 'f', 1));

	QPalette palette = ui->preview->palette();
	palette.setColor( ui->preview->backgroundRole(), this->mColor );
	ui->preview->setPalette( palette );

	ui->hdr->setVisible(this->mOverdrive > 1.0);
}

void AckColorSelector::on_showPicker_clicked()
{
	QColor qcol = this->getNormalizedColor();
	qcol = QColorDialog::getColor(qcol, this, tr("Select a color"));
	if(qcol.isValid()) {
		this->mColor = qcol;
		this->updateDisplay();
		emit colorChanged(this->color());
	}
}

void AckColorSelector::on_driver_valueChanged(int value)
{
	this->mOverdrive = value / 10.0;
	this->updateDisplay();
	emit colorChanged(this->color());
}
