#ifndef MATERIALEDITOR_HPP
#define MATERIALEDITOR_HPP

#include <acknext.h>
#include <QDockWidget>
#include <QGraphicsView>

#include "imageview.hpp"

namespace Ui {
	class MaterialEditor;
}

class MaterialEditor : public QDockWidget
{
	Q_OBJECT
	MATERIAL * mtl;
	QGraphicsScene * albedoScene;
	QGraphicsScene * normalScene;
	QGraphicsScene * attributeScene;
	QGraphicsScene * emissionScene;
public:
	explicit MaterialEditor(MATERIAL * material, QWidget *parent = 0);
	~MaterialEditor();

signals:
	Q_SIGNAL void hasChanged();

private slots:
	void on_changeAlbedoMap_clicked();

	void on_changeNormalMap_clicked();

	void on_changeAttributeMap_clicked();

	void on_changeEmissionMap_clicked();

	void on_clearAlbedoMap_clicked();

	void on_clearNormalMap_clicked();

	void on_clearAttributeMap_clicked();

	void on_clearEmissionMap_clicked();

	void on_roughness_valueChanged(int value);

	void on_metallic_valueChanged(int value);

	void on_fresnell_valueChanged(int value);

private:
	void initGui();

	void bmapToImage(ImageView * target, BITMAP const * src);

	void clearTexture(ImageView * target, BITMAP * & ptr);

	bool changeTexture(ImageView * target, BITMAP * & ptr);

private:
	Ui::MaterialEditor *ui;
};

#endif // MATERIALEDITOR_HPP
