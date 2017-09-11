#ifndef ANIMATIONVIEWER_HPP
#define ANIMATIONVIEWER_HPP

#include <acknext.h>
#include <QDockWidget>

namespace Ui {
	class AnimationViewer;
}

class AnimationViewer : public QDockWidget
{
	Q_OBJECT

public:
	explicit AnimationViewer(MODEL * model, QWidget *parent = 0);
	~AnimationViewer();

signals:
	Q_SIGNAL void hasChanged();

private:
	Ui::AnimationViewer *ui;
};

#endif // ANIMATIONVIEWER_HPP
