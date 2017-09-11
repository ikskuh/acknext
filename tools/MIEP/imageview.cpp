#include "imageview.hpp"

ImageView::ImageView(QWidget *parent) : QGraphicsView(parent)
{
	setScene(new QGraphicsScene(this));
}

void ImageView::setImage(QImage const & img)
{
	this->mImage = img;
	scene()->clear();
	if(img.isNull()) {
		return;
	}
	scene()->addPixmap(QPixmap::fromImage(img));
	scene()->setSceneRect(QRectF(0, 0, img.width(), img.height()));

	this->resetCachedContent();
	this->updateSceneRect(scene()->sceneRect());
	this->resizeEvent(nullptr);
}

void ImageView::resizeEvent(QResizeEvent *event)
{
	if(this->mImage.isNull())
		return;
	this->fitInView(0, 0, mImage.width(), mImage.height(), Qt::KeepAspectRatio);
}
