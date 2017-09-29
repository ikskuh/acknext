#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QDebug>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QOpenGLContext>
#include "qacknextwidget.hpp"

#include "materialeditor.hpp"
#include "boneeditor.hpp"
#include "meshlist.hpp"

#include "modelloader.hpp"
#include "modelsaver.hpp"

QOpenGLWidget * MainWindow::con = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    meshWidget(nullptr),
    sceneWidget(nullptr),
    animationWidget(nullptr),
    materialWidgets(),
    ui(new Ui::MainWindow)
{
	this->mLoader = new ModelLoader(this);
	ui->setupUi(this);

	this->on_actionSetMode(0);

	con = ui->centralWidget;
}

MainWindow::~MainWindow()
{
	this->cleanupModel();
	delete ui;
}

void MainWindow::openModel(MODEL * model)
{
	this->cleanupModel();

	con->makeCurrent();
	ui->centralWidget->setModel(model);

	this->materialWidgets.resize(model->meshCount);

	if(!strcmp(model->bones[0].name, "")) {
		strcpy(model->bones[0].name, "Root");
	}

	for(int i = 0; i < model->meshCount; i++)
	{
		if(!model->materials[i]) {
			model->materials[i] = mtl_create();
		}

		this->materialWidgets[i] = new MaterialEditor(model->materials[i]);
		this->materialWidgets[i]->setWindowTitle(tr("Material ") + QString::number(i));
		this->materialWidgets[i]->hide();
		connect(
			this->materialWidgets[i], SIGNAL(hasChanged()),
			ui->centralWidget, SLOT(update()));
		this->addDockWidget(Qt::RightDockWidgetArea, this->materialWidgets[i]);
	}

	this->meshWidget = new MeshList(model);
	connect(
		this->meshWidget, SIGNAL(hasChanged()),
		ui->centralWidget, SLOT(update()));
	connect(
		this->meshWidget, &MeshList::materialEditorRequested,
		this, &MainWindow::showMaterialEditor);
	this->addDockWidget(Qt::LeftDockWidgetArea, this->meshWidget);

	this->sceneWidget = new BoneEditor(model);
	connect(
		this->sceneWidget, SIGNAL(hasChanged()),
		ui->centralWidget, SLOT(update()));
	this->addDockWidget(Qt::LeftDockWidgetArea, this->sceneWidget);

	this->animationWidget = new AnimationViewer(ui->centralWidget, model);
	connect(
		this->animationWidget, SIGNAL(hasChanged()),
		ui->centralWidget, SLOT(update()));
	this->addDockWidget(Qt::LeftDockWidgetArea, this->animationWidget);

	if(model->boneCount <= 1)
		this->sceneWidget->hide();

	if(model->animationCount == 0)
		this->animationWidget->hide();
}

void MainWindow::cleanupModel()
{
	if(ui->centralWidget->model() == nullptr)
		return;
	con->makeCurrent();
	delete this->meshWidget;
	delete this->sceneWidget;
	delete this->animationWidget;
	for(auto * ptr : this->materialWidgets)
		delete ptr;
	this->materialWidgets.clear();
	model_remove(ui->centralWidget->model());
	ui->centralWidget->setModel(nullptr);
}

void MainWindow::updateVisuals()
{
	ui->centralWidget->setDisplayMode(
		ui->actionShow_Mesh->isChecked(),
		ui->actionShow_Skelton->isChecked(),
		ui->actionShow_normals->isChecked());
	ui->centralWidget->update();
}

void MainWindow::showMaterialEditor(int i)
{
	if(i >= 0 && i < this->materialWidgets.size()) {
		this->materialWidgets[i]->show();
	}
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("Open model file..."),
        QString(),
		tr("Model Files (*.amd)"));
	if(fileName.isNull()) {
		return;
	}

	ui->centralWidget->makeCurrent();

	auto name = fileName.toStdString();
	MODEL * model = model_load(name.c_str());
	if(model != nullptr) {
		this->openModel(model);
	} else {
		QMessageBox::critical(
			this,
			this->windowTitle(),
			tr("Failed to open file: %1").arg(engine_lasterror_text));
	}

	ui->centralWidget->doneCurrent();
}


void MainWindow::on_actionScene_View_triggered()
{
	this->on_actionSetMode(0);
}

void MainWindow::on_actionLight_View_triggered()
{
	this->on_actionSetMode(1);
}

void MainWindow::on_actionNormal_View_triggered()
{
	this->on_actionSetMode(2);
}

void MainWindow::on_actionSetMode(int mode)
{
	this->ui->actionScene_View->setChecked(mode == 0);
	this->ui->actionLight_View->setChecked(mode == 1);
	this->ui->actionNormal_View->setChecked(mode == 2);
	opengl_debugMode = mode;
	ui->centralWidget->update();
}

void MainWindow::on_actionWireframe_triggered()
{
	opengl_wireFrame = this->ui->actionWireframe->isChecked();
	ui->centralWidget->update();
}

void MainWindow::on_actionShow_Mesh_triggered(bool checked)
{
	Q_UNUSED(checked);
	this->updateVisuals();
}

void MainWindow::on_actionShow_Skelton_triggered(bool checked)
{
	Q_UNUSED(checked);
	this->updateVisuals();
}

void MainWindow::on_actionSave_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(
		this,
		tr("Open model file..."),
		QString(),
		tr("Model Files (*.amd)"));
	if(fileName.isNull()) {
		return;
	}

	auto str = fileName.toStdString();

	ACKFILE * file = file_open_write(str.c_str());
	ModelSaver::write(file, ui->centralWidget->model());
	file_close(file);
}

void MainWindow::on_actionImport_model_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(
		this,
		"Open model file...");
	if(fileName.isNull()) {
		return;
	}

	ui->centralWidget->makeCurrent();

	MODEL * model = loader()->load(fileName);
	if(model != nullptr) {
		this->openModel(model);
	} else {
		QMessageBox::critical(
			this,
			this->windowTitle(),
			tr("Failed to open file: %1").arg(engine_lasterror_text));
	}

	ui->centralWidget->doneCurrent();
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionShow_normals_triggered(bool checked)
{
	Q_UNUSED(checked);
	this->updateVisuals();
}
