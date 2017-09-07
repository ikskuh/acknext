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

private slots:
	void on_showMaterial_clicked();

	void on_toggleMeshVis_clicked();

	void on_openTools_clicked();

	void on_deleteMesh_clicked();

	void on_list_activated(const QModelIndex &index);

private:
	void setupGui();

private:
	Ui::MeshList *ui;
};

#endif // MESHLIST_HPP
