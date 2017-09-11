#ifndef BONEEDITOR_HPP
#define BONEEDITOR_HPP

#include <acknext.h>
#include <QDockWidget>

#include "bonetreemodel.hpp"

namespace Ui {
	class BoneEditor;
}

class BoneEditor : public QDockWidget
{
	Q_OBJECT
	MODEL * model;
public:
	explicit BoneEditor(MODEL * model, QWidget *parent = 0);
	~BoneEditor();

signals:
	Q_SIGNAL void hasChanged();

private slots:
	void on_name_editingFinished();

private:
	void setupGui();

	void selectBone(int index);

	void on_tree_selectionChanged(const QModelIndex &current, const QModelIndex &previous);

private:
	Ui::BoneEditor *ui;
	BoneTreeModel *boneModel;
	int currentBone;
};

#endif // BONEEDITOR_HPP
