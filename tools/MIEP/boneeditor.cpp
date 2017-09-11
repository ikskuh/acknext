#include "boneeditor.hpp"
#include "ui_boneeditor.h"

#include <QTreeView>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>

#include <string.h>
#include <strings.h>

static QString toString(MATRIX const & mat)
{
	QString format(
		"[%1 %2 %3 %4\n"
		" %5 %6 %7 %8\n"
		" %9 %10 %11 %12\n"
		" %13 %14 %15 %16 ]");
	const int pad = 6;
	return format
		.arg(mat.fields[0][0], pad, 'f', 3)
		.arg(mat.fields[0][1], pad, 'f', 3)
		.arg(mat.fields[0][2], pad, 'f', 3)
		.arg(mat.fields[0][3], pad, 'f', 3)
		.arg(mat.fields[1][0], pad, 'f', 3)
		.arg(mat.fields[1][1], pad, 'f', 3)
		.arg(mat.fields[1][2], pad, 'f', 3)
		.arg(mat.fields[1][3], pad, 'f', 3)
		.arg(mat.fields[2][0], pad, 'f', 3)
		.arg(mat.fields[2][1], pad, 'f', 3)
		.arg(mat.fields[2][2], pad, 'f', 3)
		.arg(mat.fields[2][3], pad, 'f', 3)
		.arg(mat.fields[3][0], pad, 'f', 3)
		.arg(mat.fields[3][1], pad, 'f', 3)
		.arg(mat.fields[3][2], pad, 'f', 3)
		.arg(mat.fields[3][3], pad, 'f', 3);
}

BoneEditor::BoneEditor(MODEL * model, QWidget *parent) :
    QDockWidget(parent),
    model(model),
    ui(new Ui::BoneEditor),
    currentBone(-1)
{
	Q_ASSERT(model);
	ui->setupUi(this);
	this->setupGui();
}

BoneEditor::~BoneEditor()
{
	delete ui;
}

void BoneEditor::setupGui()
{
	this->boneModel = new BoneTreeModel(this->model, this);
	ui->tree->setModel(this->boneModel);
	ui->tree->setRootIndex(this->boneModel->root());

	connect(
		ui->tree->selectionModel(),
		&QItemSelectionModel::currentChanged,
		this,
		&BoneEditor::on_tree_selectionChanged);

	this->selectBone(-1);
}

void BoneEditor::selectBone(int index)
{
	this->currentBone = index;
	ui->name->setEnabled(index >= 0);
	if(!ui->name->isEnabled()) {
		ui->name->setText("");
		ui->transform->setText(toString(*mat_id(NULL)));
		return;
	}
	BONE const & bone = model->bones[index];
	ui->name->setText(QString::fromUtf8(bone.name));
	ui->transform->setText(toString(bone.transform));
}

void BoneEditor::on_name_editingFinished()
{
	Q_ASSERT(this->currentBone >= 0 && this->currentBone < model->boneCount);

	QByteArray data;
	QString name = ui->name->text();
	do {
		data = name.toUtf8();
		name.chop(1);
	} while(data.size() >= sizeof(BONE::name));

	memset(
		model->bones[this->currentBone].name,
	    0,
		sizeof(BONE::name));
	memcpy(
		model->bones[this->currentBone].name,
		data.data(),
		data.size());

	boneModel->refreshBone(this->currentBone);

	emit this->hasChanged();
}

void BoneEditor::on_tree_selectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
	Q_UNUSED(previous);
	if(current.isValid()) {
		int index = boneModel->boneIndex(current);
		if(index >= 0) {
			this->selectBone(index);
			emit this->hasChanged();
		}
	}
}
