#ifndef ANIMATIONVIEWER_HPP
#define ANIMATIONVIEWER_HPP

#include <acknext.h>
#include <QDockWidget>

#include "modelanimationlistmodel.hpp"

namespace Ui {
	class AnimationViewer;
}

class AnimationViewer : public QDockWidget
{
	Q_OBJECT
private:
	MODEL * model;
	ModelAnimationListModel * listModel;
	ANIMATION * selection;
public:
	explicit AnimationViewer(MODEL * model, QWidget *parent = 0);
	~AnimationViewer();

signals:
	Q_SIGNAL void hasChanged();

private slots:
	void on_list_currentChanged(const QModelIndex &current, const QModelIndex &previous);

	void on_progress_valueChanged(int value);

private:
	void selectAnim(ANIMATION * anim);
	void updatePosition();

	void animate(double frameTime);

private:
	Ui::AnimationViewer *ui;
};

#endif // ANIMATIONVIEWER_HPP
