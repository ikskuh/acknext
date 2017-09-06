#include "materialeditor.hpp"
#include "ui_materialeditor.h"

#include <QGraphicsScene>
#include <QSlider>
#include <QMessageBox>

static float map(float v, float srcMin, float srcMax, float dstMin, float dstMax)
{
	return (dstMax - dstMin) * (v - srcMin) / (srcMax - srcMin)	+ dstMin;
}

MaterialEditor::MaterialEditor(MATERIAL * material, QWidget *parent) :
    QDockWidget(parent),
    mtl(material),
    ui(new Ui::MaterialEditor)
{
	albedoScene = new QGraphicsScene(this);
	normalScene = new QGraphicsScene(this);
	attributeScene = new QGraphicsScene(this);
	emissionScene = new QGraphicsScene(this);

	ui->setupUi(this);
	this->initGui();
}

MaterialEditor::~MaterialEditor()
{
	delete ui;
}

void MaterialEditor::initGui()
{
	ui->albedoMap->setScene(albedoScene);
	ui->normalMap->setScene(normalScene);
	ui->attributeMap->setScene(attributeScene);
	ui->emissionMap->setScene(emissionScene);

	bmapToImage(ui->albedoMap, mtl->albedoTexture);
	bmapToImage(ui->normalMap, mtl->normalTexture);
	bmapToImage(ui->attributeMap, mtl->attributeTexture);
	bmapToImage(ui->emissionMap, mtl->emissionTexture);

	ui->roughness->setValue(map(mtl->roughness, 0, 1, 0, 100));
	ui->metallic->setValue(map(mtl->metallic, 0, 1, 0, 100));
	ui->fresnell->setValue(map(mtl->fresnell, 0, 100, 0, 100));
}

void MaterialEditor::bmapToImage(ImageView * target, BITMAP const * src)
{
	if(src) {
		QImage img((uchar*)src->pixels, src->width, src->height, QImage::Format_RGBA8888);
		target->setImage(img.rgbSwapped());
	} else {
		target->setImage(QImage());
	}
}

void MaterialEditor::clearTexture(ImageView *target, BITMAP *&ptr)
{
	bmap_remove(ptr);
	ptr = nullptr;
	bmapToImage(target, nullptr);
}

bool MaterialEditor::changeTexture(ImageView *target, BITMAP *&ptr)
{
	QMessageBox::warning(this, this->windowTitle(), "texture change not implemented yet!");
	return false;
}

void MaterialEditor::on_changeAlbedoMap_clicked()
{
	if(this->changeTexture(ui->albedoMap, mtl->albedoTexture)) {
		emit this->hasChanged();
	}
}

void MaterialEditor::on_changeNormalMap_clicked()
{
	if(this->changeTexture(ui->normalMap, mtl->normalTexture)) {
		emit this->hasChanged();
	}
}

void MaterialEditor::on_changeAttributeMap_clicked()
{
	if(this->changeTexture(ui->attributeMap, mtl->attributeTexture)) {
		emit this->hasChanged();
	}
}

void MaterialEditor::on_changeEmissionMap_clicked()
{
	if(this->changeTexture(ui->emissionMap, mtl->emissionTexture)) {
		emit this->hasChanged();
	}
}

void MaterialEditor::on_clearAlbedoMap_clicked()
{
	clearTexture(ui->albedoMap, mtl->albedoTexture);
	emit this->hasChanged();
}

void MaterialEditor::on_clearNormalMap_clicked()
{
	clearTexture(ui->normalMap, mtl->normalTexture);
	emit this->hasChanged();
}

void MaterialEditor::on_clearAttributeMap_clicked()
{
	clearTexture(ui->attributeMap, mtl->attributeTexture);
	emit this->hasChanged();
}

void MaterialEditor::on_clearEmissionMap_clicked()
{
	clearTexture(ui->emissionMap, mtl->emissionTexture);
	emit this->hasChanged();
}

void MaterialEditor::on_roughness_valueChanged(int value)
{
	mtl->roughness = map(ui->roughness->value(), 0, 100, 0, 1);
    emit this->hasChanged();
}

void MaterialEditor::on_metallic_valueChanged(int value)
{
	mtl->metallic = map(ui->metallic->value(), 0, 100, 0, 1);
	emit this->hasChanged();
}

void MaterialEditor::on_fresnell_valueChanged(int value)
{
	mtl->fresnell = map(ui->fresnell->value(), 0, 100, 0, 100);
	emit this->hasChanged();
}
