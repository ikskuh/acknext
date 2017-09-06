#ifndef IMAGEVIEW_HPP
#define IMAGEVIEW_HPP

#include <QGraphicsView>

class ImageView : public QGraphicsView
{
	Q_OBJECT
	QImage mImage;
public:
	explicit ImageView(QWidget *parent = nullptr);

	void setImage(QImage const & img);

	QImage const & image() const {
		return this->mImage;
	}

	virtual void resizeEvent(QResizeEvent *event) override;

signals:

public slots:
};

#endif // IMAGEVIEW_HPP
