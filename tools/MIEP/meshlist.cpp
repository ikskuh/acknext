#include "meshlist.hpp"
#include "ui_meshlist.h"

#include <QToolButton>
#include <QListView>
#include <QLayout>

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

	connect(
		ui->list->selectionModel(), &QItemSelectionModel::currentChanged,
		this, &MeshList::on_list_selectionChanged);

	connect(
		ui->bitmask, &BitMaskEditor::hasChanged,
		this, &MeshList::hasChanged);

	ui->showMaterial->setEnabled(false);
	ui->toggleMeshVis->setEnabled(false);
	ui->openTools->setEnabled(false);
	ui->deleteMesh->setEnabled(false);
}


void MeshList::selectIndex(int i)
{
	auto en = (i >= 0);
	ui->showMaterial->setEnabled(en);
	ui->deleteMesh->setEnabled(en);
	ui->openTools->setEnabled(en);
	ui->toggleMeshVis->setEnabled(en);

	this->ui->bitmask->setMesh(this->model->meshes[i]);
}

void MeshList::on_list_selectionChanged(const QModelIndex & current, const QModelIndex & previous)
{
	if(current.isValid())
		this->selectIndex(current.row());
	else
		this->selectIndex(-1);
}

void MeshList::on_showMaterial_clicked()
{
	auto selmod = ui->list->selectionModel();
	auto list = selmod->selectedIndexes();
	if(list.size() > 0) {
		auto index = list.first();
		emit materialEditorRequested(index.row());
	}
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
	if(index.isValid()) {
		emit materialEditorRequested(index.row());
	}
}
