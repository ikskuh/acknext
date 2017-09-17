#include "bitmaskeditor.hpp"
#include "ui_bitmaskeditor.h"

#include <QSlider>
#include <QLabel>
#include <QDebug>
#include <QCheckBox>

BitMaskEditor::BitMaskEditor(QWidget *parent) :
    QWidget(parent),
    mesh(nullptr),
    ui(new Ui::BitMaskEditor)
{
	ui->setupUi(this);
	this->setMesh(nullptr);
}

BitMaskEditor::~BitMaskEditor()
{
	delete ui;
}

void BitMaskEditor::setMesh(MESH *mesh)
{
	this->mesh = mesh;
	this->setEnabled(this->mesh != nullptr);

	if(this->mesh)
	{
		qDebug() << "got mesh" << mesh->lodMask;

		bool hasMin = false;
		int min = 0;
		int max = 15;
		for(int i = 0; i < 16; i++) {
			if(hasMin) {
				if(mesh->lodMask & (1<<i)) {
					max = i;
				}
			} else {
				if(mesh->lodMask & (1<<i)) {
					min = i;
					hasMin = true;
				}
			}
		}

		this->ui->checkBox->setChecked(mesh->lodMask & DOUBLESIDED);
		this->ui->minimum->setValue(min);
		this->ui->maximum->setValue(15 - max);

		this->updateMesh();
	}
}

void BitMaskEditor::updateMesh()
{
	uint32_t mask = 0;
	for(int i = 0; i < 16; i++) {
		if(i >= ui->minimum->value() && i <= (15 - ui->maximum->value()))
			mask |= (1<<i);
	}
	if(ui->checkBox->isChecked()) {
		mask |= DOUBLESIDED;
	}

	this->ui->label->setText(
		QString("%1 (%2 m) ≤ LOD ≤ %3 (%4 m)\n%5")
			.arg(ui->minimum->value())
	        .arg(lod_distances[ui->minimum->value()])
			.arg(15 - ui->maximum->value())
	        .arg(lod_distances[15 - ui->maximum->value()])
			.arg(mask & 0xFFFF, 16, 2, QChar('0')));

	if(mesh) {
		mesh->lodMask = mask;
		emit this->hasChanged();
	}
}

void BitMaskEditor::on_minimum_valueChanged(int value)
{
	int max = 15 - ui->maximum->value();
	if(max < value) max = value;
	ui->maximum->setValue(15 - max);
	this->updateMesh();
}

void BitMaskEditor::on_maximum_valueChanged(int value)
{
	int max = 15 - ui->minimum->value();
	if(max < value) max = value;
	ui->minimum->setValue(15 - max);
	this->updateMesh();
}

void BitMaskEditor::on_checkBox_stateChanged(int arg1)
{
	this->updateMesh();
}
