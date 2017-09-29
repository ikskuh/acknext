#ifndef ANIMATIONVIEWER_HPP
#define ANIMATIONVIEWER_HPP

#include <acknext.h>
#include <QDockWidget>
#include <QTimer>

#include "modelanimationlistmodel.hpp"
#include "qacknextwidget.hpp"

namespace Ui {
	class AnimationViewer;
}

class AnimationViewer : public QDockWidget
{
	Q_OBJECT
private:
	QAcknextWidget * acknex;
	MODEL * model;
	ModelAnimationListModel * listModel;
	ANIMATION * selection;
	QTimer * timer;
	double progress;
public:
	explicit AnimationViewer(QAcknextWidget * acknex, MODEL * model, QWidget *parent = 0);
	~AnimationViewer();

	virtual void closeEvent(QCloseEvent *event) override;

signals:
	Q_SIGNAL void hasChanged();

private slots:
	void on_list_currentChanged(const QModelIndex &current, const QModelIndex &previous);

	void on_progress_valueChanged(int value);

	void on_play_clicked();

	void on_stop_clicked();

	void on_rewind_clicked();

	void on_looped_clicked(bool checked);

	void on_list_doubleClicked(const QModelIndex &index);

	void on_name_editingFinished();

private:
	void selectAnim(ANIMATION * anim);

	void animate(double frameTime);

	void animateFurther();

private:
	Ui::AnimationViewer *ui;
};

#endif // ANIMATIONVIEWER_HPP
