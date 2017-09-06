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

	QAcknextWidget * ack = new QAcknextWidget(ui->mdiArea);
	ack->show();
	ack->context()->setShareContext(con->context());

	ack->makeCurrent();

	MODEL * model = model_load(fileName.toUtf8().data());

	if(model == nullptr) {
		QMessageBox::critical(
			this,
			this->windowTitle(),
			tr("Failed to open file: %1").arg(engine_lasterror_text));
		delete ack;
		ack->doneCurrent();
		return;
	}

	if(ack->model() != nullptr)
	{
		model_remove(ack->model());
	}

	ack->setModel(model);

	ack->doneCurrent();

	auto iwn = this->ui->mdiArea->addSubWindow(ack);

	iwn->show();
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
	auto win = this->ui->mdiArea->currentSubWindow();
	if(win)
	{
		QAcknextWidget * ack = (QAcknextWidget*)win->widget();
		this->ui->actionScene_View->setChecked(mode == 0);
		this->ui->actionLight_View->setChecked(mode == 1);
		this->ui->actionNormal_View->setChecked(mode == 2);
		opengl_debugMode = mode;
		ack->update();
	}
}

void MainWindow::on_actionWireframe_triggered()
{
	auto win = this->ui->mdiArea->currentSubWindow();
	if(win) {
		QAcknextWidget * ack = (QAcknextWidget*)win->widget();
		opengl_wireFrame = this->ui->actionWireframe->isChecked();
		ack->update();
	}
}

void MainWindow::on_actionInit_triggered()
{
	con = new Daaang();
	auto win = ui->mdiArea->addSubWindow(con);
	win->show();
}
