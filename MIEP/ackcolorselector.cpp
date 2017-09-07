#include "ackcolorselector.hpp"
#include "ui_ackcolorselector.h"

#include <QPalette>
#include <QColorDialog>
#include <QFrame>
#include <QLabel>

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

void AckColorSelector::setColor(COLOR color)
{
	this->mColor = color;

	QColor qcol = QColor::fromRgbF(color.red, color.green, color.blue, color.alpha);

	QPalette palette = ui->preview->palette();
	palette.setColor( ui->preview->backgroundRole(), qcol );
	ui->preview->setPalette( palette );

	ui->hdr->setVisible(color.red > 1.0
		|| color.green > 1.0
		|| color.blue > 1.0
		|| color.alpha > 1.0);

	emit colorChanged(color);
}

void AckColorSelector::on_showPicker_clicked()
{
	QColor qcol = QColor::fromRgbF(mColor.red, mColor.green, mColor.blue, mColor.alpha);
	qcol = QColorDialog::getColor(qcol, this, tr("Select a color"));
	if(qcol.isValid()) {
		this->setColor((COLOR) {
			qcol.redF(),
			qcol.greenF(),
			qcol.blueF(),
			qcol.alphaF(),
		});
	}
}
