#ifndef QACKNEXTWIDGET_HPP
#define QACKNEXTWIDGET_HPP

#include <acknext.h>
#include <QOpenGLWidget>

class QAcknextWidget : public QOpenGLWidget
{
	Q_OBJECT
private:
	ENTITY * mModelDisplay;
	VIEW * mView;
	LIGHT * mSun;
	QPoint mPrevPos;
	bool mShowSkeleton;
public:
	explicit QAcknextWidget(QWidget *parent = nullptr);
	~QAcknextWidget();

	virtual void initializeGL() override;

	virtual void resizeGL(int w, int h) override;

	virtual void paintGL() override;

	virtual void mousePressEvent(QMouseEvent *event) override;

	virtual void mouseMoveEvent(QMouseEvent *event) override;

	virtual void wheelEvent(QWheelEvent *event) override;

	void setModel(MODEL * model);

	MODEL * model() const { return this->mModelDisplay->model; }

	void setDisplayMode(bool mesh, bool skeleton) {
		this->mModelDisplay->flags = (this->mModelDisplay->flags & ~VISIBLE) | (mesh?VISIBLE:0);
		this->mShowSkeleton = skeleton;
	}

	bool displaysMesh() { return this->mModelDisplay->flags & VISIBLE; }

	bool displaysSkeleton() { return this->mShowSkeleton; }

private:
	void drawBones();

signals:

public slots:
};

#endif // QACKNEXTWIDGET_HPP
