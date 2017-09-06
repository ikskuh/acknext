#ifndef DAAANG_HPP
#define DAAANG_HPP

#include <acknext.h>
#include <QOpenGLWidget>

class Daaang : public QOpenGLWidget
{
	Q_OBJECT
public:
	explicit Daaang(QWidget *parent = nullptr);

	virtual void initializeGL() override;

signals:
	Q_SIGNAL void done();

public slots:
};

#endif // DAAANG_HPP
