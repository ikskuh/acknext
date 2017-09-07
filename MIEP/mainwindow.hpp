#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <acknext.h>
#include <QMainWindow>
#include <QOpenGLWidget>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_actionOpen_triggered();

	void on_actionNormal_View_triggered();

	void on_actionLight_View_triggered();

	void on_actionScene_View_triggered();

	void on_actionWireframe_triggered();

private:
	void on_actionSetMode(int mode);

private:
	Ui::MainWindow *ui;
	static QOpenGLWidget * con;

public:
	static void makeCurrent() { con->makeCurrent(); }
	static void doneCurrent() { con->doneCurrent(); }
};

#endif // MAINWINDOW_HPP
