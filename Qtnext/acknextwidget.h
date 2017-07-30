#ifndef ACKNEXTWIDGET_H
#define ACKNEXTWIDGET_H

#include <QOpenGLWidget>

class AcknextWidget : public QOpenGLWidget
{
	Q_OBJECT
public:
	explicit AcknextWidget(QWidget *parent = nullptr);

	virtual void initializeGL() override;

	virtual void paintGL() override;

signals:

public slots:
};

#endif // ACKNEXTWIDGET_H
