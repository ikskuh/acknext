#include "meshlist.hpp"
#include "ui_meshlist.h"

#include <QToolButton>
#include <QListView>
#include <QLayout>

#include "mainwindow.hpp"

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

MESH * MeshList::currentMesh() const
{
	int index = ui->list->selectionModel()->currentIndex().row();
	if(index >= 0 && index < model->meshCount)
		return model->meshes[index];
	else
		return nullptr;
}

void MeshList::selectIndex(int i)
{
	auto en = (i >= 0);
	ui->showMaterial->setEnabled(en);
	ui->deleteMesh->setEnabled(en);
	ui->openTools->setEnabled(en);
	ui->toggleMeshVis->setEnabled(en);

	if(en) {
		this->ui->bitmask->setMesh(this->model->meshes[i]);
	} else {
		this->ui->bitmask->setMesh(nullptr);
	}
}

void MeshList::on_list_selectionChanged(const QModelIndex & current, const QModelIndex & previous)
{
	Q_UNUSED(previous);
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
	MainWindow::makeCurrent();

	MESH * mesh = this->currentMesh();
	if(!mesh) return;

	VERTEX * vertices = (VERTEX*)buffer_map(mesh->vertexBuffer, READWRITE);
	INDEX  * indices =  (INDEX*)buffer_map(mesh->indexBuffer, READONLY);
	int indexCount = mesh->indexBuffer->size / sizeof(INDEX);
	int vertexCount = mesh->vertexBuffer->size / sizeof(VERTEX);

	engine_log(
		"%d→%d, %d→%d",
		mesh->vertexBuffer->size,
		vertexCount,
		mesh->indexBuffer->size,
		indexCount);

	for(int i = 0; i < vertexCount; i++) {
		vertices[i].normal = nullvector;
	}
	for(int i = 0; i < indexCount; i += 3) {
		int a,b,c;
		VECTOR va,vb,vc;

		a = indices[i + 0];
		b = indices[i + 1];
		c = indices[i + 2];

		va = vertices[a].position;
		vb = vertices[b].position;
		vc = vertices[c].position;

		vec_sub(&vb, &va);
		vec_sub(&vc, &va);

		vec_normalize(&vb, 1.0);
		vec_normalize(&vc, 1.0);

		VECTOR normal;
		vec_cross(&normal, &vb, &vc);
		vec_normalize(&normal, 1.0);

		vec_add(&vertices[a].normal, &normal);
		vec_add(&vertices[b].normal, &normal);
		vec_add(&vertices[c].normal, &normal);
	}
	for(int i = 0; i < vertexCount; i++) {
		vec_normalize(&vertices[i].normal, 1.0);

		if(vec_length(&vertices[i].tangent) < 0.5) {
			VECTOR tangent;
			vec_cross(&tangent, &vertices[i].normal, vector(0, 1, 0));
			if(vec_length(&tangent) < 0.5)
				vec_cross(&tangent, &vertices[i].normal, vector(1, 0, 0));
			vec_normalize(&tangent, 1.0);
			vertices[i].tangent = tangent;
		}
	}

	buffer_unmap(mesh->vertexBuffer);
	buffer_unmap(mesh->indexBuffer);


	emit this->hasChanged();
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
