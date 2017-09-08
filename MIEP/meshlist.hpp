#ifndef MESHLIST_HPP
#define MESHLIST_HPP

#include <acknext.h>
#include <QDockWidget>

#include "meshlistmodel.hpp"

namespace Ui {
	class MeshList;
}

class MeshList : public QDockWidget
{
	Q_OBJECT
	MODEL * model;
	MeshListModel * meshlist;
public:
	explicit MeshList(MODEL * model, QWidget *parent = 0);
	~MeshList();

signals:
	Q_SIGNAL void hasChanged();

	Q_SIGNAL void materialEditorRequested(int materialIndex);

private slots:
	void on_showMaterial_clicked();

	void on_toggleMeshVis_clicked();

	void on_openTools_clicked();

	void on_deleteMesh_clicked();

	void on_list_activated(const QModelIndex &index);

	void on_list_selectionChanged(const QModelIndex & current, const QModelIndex & previous);

private:
	void setupGui();

	void selectIndex(int i);

private:
	Ui::MeshList *ui;
};

#endif // MESHLIST_HPP
