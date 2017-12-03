#include "materialeditor.hpp"
#include "ui_materialeditor.h"

#include <QGraphicsScene>
#include <QSlider>
#include <QMessageBox>
#include <QFileDialog>

#include "mainwindow.hpp"

static float map(float v, float srcMin, float srcMax, float dstMin, float dstMax)
{
	return (dstMax - dstMin) * (v - srcMin) / (srcMax - srcMin)	+ dstMin;
}

#include <QImage>
#include "math.h"

template<class T>
inline const T& kClamp( const T& x, const T& low, const T& high )
{
    if ( x < low )       return low;
    else if ( high < x ) return high;
    else                 return x;
}

inline
int changeGamma( int value, int gamma )
    {
    return kClamp( int( pow( value / 255.0, 100.0 / gamma ) * 255 ), 0, 255 );
    }

inline
int changeUsingTable( int value, const int table[] )
    {
    return table[ value ];
    }

template< int operation( int, int ) >
static
QImage changeImage( const QImage& image, int value )
    {
    QImage im = image;
    im.detach();
    if( im.colorCount() == 0 ) /* truecolor */
        {
        if( im.format() != QImage::Format_RGB32 ) /* just in case */
            im = im.convertToFormat( QImage::Format_RGB32 );
        int table[ 256 ];
        for( int i = 0;
             i < 256;
             ++i )
            table[ i ] = operation( i, value );
        if( im.hasAlphaChannel() )
            {
            for( int y = 0;
                 y < im.height();
                 ++y )
                {
                QRgb* line = reinterpret_cast< QRgb* >( im.scanLine( y ));
                for( int x = 0;
                     x < im.width();
                     ++x )
                    line[ x ] = qRgba( changeUsingTable( qRed( line[ x ] ), table ),
                        changeUsingTable( qGreen( line[ x ] ), table ),
                        changeUsingTable( qBlue( line[ x ] ), table ),
                        changeUsingTable( qAlpha( line[ x ] ), table ));
                }
            }
        else
            {
            for( int y = 0;
                 y < im.height();
                 ++y )
                {
                QRgb* line = reinterpret_cast< QRgb* >( im.scanLine( y ));
                for( int x = 0;
                     x < im.width();
                     ++x )
                    line[ x ] = qRgb( changeUsingTable( qRed( line[ x ] ), table ),
                        changeUsingTable( qGreen( line[ x ] ), table ),
                        changeUsingTable( qBlue( line[ x ] ), table ));
                }
            }
        }
    else
        {
        QVector<QRgb> colors = im.colorTable();
        for( int i = 0;
             i < im.colorCount();
             ++i )
            colors[ i ] = qRgb( operation( qRed( colors[ i ] ), value ),
                operation( qGreen( colors[ i ] ), value ),
                operation( qBlue( colors[ i ] ), value ));
        }
    return im;
}

// gamma is multiplied by 100 in order to avoid floating point numbers
QImage changeGamma( const QImage& image, int gamma )
    {
    if( gamma == 100 ) // no change
        return image;
    return changeImage< changeGamma >( image, gamma );
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

	connect(
		ui->albedo, &AckColorSelector::colorChanged,
		this, &MaterialEditor::on_albedo_colorChanged);
	connect(
		ui->emission, &AckColorSelector::colorChanged,
		this, &MaterialEditor::on_emission_colorChanged);

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

	ui->albedo->setColor(mtl->albedo);
	ui->emission->setColor(mtl->emission);

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
		target->setImage(changeGamma(img.mirrored(false, true), 220));
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
	QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Select a texture file"),
		QString(),
		tr("Texture Files (*.png *.jpg *.bmp)"));
	if(fileName.isNull()) {
		return false;
	}
	QImage source(fileName);
	QImage fitting = changeGamma(source
		.convertToFormat(QImage::Format_ARGB32)
		.rgbSwapped()
		.mirrored(false, true),
		45);

	MainWindow::makeCurrent();

	if(ptr != nullptr) {
		bmap_remove(ptr);
	}

	ptr = bmap_create(GL_TEXTURE_2D, GL_RGBA8);
	const_cast<void*&>(ptr->pixels) = malloc(4 * fitting.width() * fitting.height());
	memcpy(ptr->pixels, fitting.bits(), 4 * fitting.width() * fitting.height());

	bmap_set(
		ptr,
		fitting.width(), fitting.height(),
		GL_RGBA, GL_UNSIGNED_BYTE,
		ptr->pixels);

	bmapToImage(target, ptr);

	MainWindow::doneCurrent();

	return true;
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
	mtl->roughness = map(value, 0, 100, 0, 1);
    emit this->hasChanged();
}

void MaterialEditor::on_metallic_valueChanged(int value)
{
	mtl->metallic = map(value, 0, 100, 0, 1);
	emit this->hasChanged();
}

void MaterialEditor::on_fresnell_valueChanged(int value)
{
	mtl->fresnell = map(value, 0, 100, 0, 100);
	emit this->hasChanged();
}


void MaterialEditor::on_albedo_colorChanged(COLOR newColor)
{
	mtl->albedo = newColor;
	emit this->hasChanged();
}

void MaterialEditor::on_emission_colorChanged(COLOR newColor)
{
	mtl->emission = newColor;
	emit this->hasChanged();
}
