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
#include "daaang.hpp"

#include "materialeditor.hpp"
#include "boneeditor.hpp"
#include "meshlist.hpp"

QOpenGLWidget * MainWindow::con = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->on_actionSetMode(0);

	con = ui->centralWidget;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
		this,
		"Open model file...");
	if(fileName.isNull()) {
		return;
	}

	ui->centralWidget->makeCurrent();

	MODEL * model = model_load(fileName.toUtf8().data());
	if(model != nullptr) {
		if(ui->centralWidget->model() != nullptr)
		{
			model_remove(ui->centralWidget->model());
		}
		ui->centralWidget->setModel(model);

		for(int i = 0; i < model->meshCount; i++)
		{
			if(model->materials[i]) {
				auto dock = new MaterialEditor(model->materials[i]);
				dock->setWindowTitle(tr("Material ") + QString::number(i));
				connect(
					dock, SIGNAL(hasChanged()),
					ui->centralWidget, SLOT(update()));
				this->addDockWidget(Qt::RightDockWidgetArea, dock);
			}
		}

		{
			auto meshed = new MeshList(model);
			connect(
				meshed, SIGNAL(hasChanged()),
				ui->centralWidget, SLOT(update()));
			this->addDockWidget(Qt::LeftDockWidgetArea, meshed);
		}
		{
			auto boned = new BoneEditor(model);
			connect(
				boned, SIGNAL(hasChanged()),
				ui->centralWidget, SLOT(update()));
			this->addDockWidget(Qt::LeftDockWidgetArea, boned);
		}

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
