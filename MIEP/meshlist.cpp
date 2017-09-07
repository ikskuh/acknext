#include "meshlist.hpp"
#include "ui_meshlist.h"

#include <QToolButton>
#include <QListView>

MeshList::MeshList(MODEL * model, QWidget *parent) :
    QDockWidget(parent),
    model(model),
    ui(new Ui::MeshList)
{
	ui->setupUi(this);
	this->setupGui();
}

MeshList::~MeshList()
{
	delete this->meshlist;
	delete ui;
}

void MeshList::setupGui()
{
	this->meshlist = new MeshListModel(model);

	ui->list->setModel(this->meshlist);

	ui->showMaterial->setEnabled(false);
	ui->toggleMeshVis->setEnabled(false);
	ui->openTools->setEnabled(false);
	ui->deleteMesh->setEnabled(false);
}

void MeshList::on_showMaterial_clicked()
{

}

void MeshList::on_toggleMeshVis_clicked()
{

}

void MeshList::on_openTools_clicked()
{

}

void MeshList::on_deleteMesh_clicked()
{

}

void MeshList::on_list_activated(const QModelIndex &index)
{
    engine_log("Selected %d", index.row());
}
