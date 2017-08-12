#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QAction>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->on_actionSetMode(0);
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
	ui->centralWidget->doneCurrent();

	if(model == nullptr) {
		QMessageBox::critical(
			this,
			this->windowTitle(),
			tr("Failed to open file: %1").arg(engine_lasterror_text));
		return;
	}

	if(this->ui->centralWidget->model() != nullptr)
	{
		model_remove(this->ui->centralWidget->model());
	}

	this->ui->centralWidget->setModel(model);
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
	this->ui->centralWidget->update();
}

void MainWindow::on_actionWireframe_triggered()
{
	opengl_wireFrame = this->ui->actionWireframe->isChecked();
    this->ui->centralWidget->update();
}
