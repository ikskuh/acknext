#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <acknext.h>
#include <QMainWindow>
#include <QOpenGLWidget>

#include "materialeditor.hpp"
#include "meshlist.hpp"
#include "animationviewer.hpp"
#include "boneeditor.hpp"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	MeshList * meshWidget;
	BoneEditor * sceneWidget;
	AnimationViewer * animationWidget;
	QVector<MaterialEditor*> materialWidgets;
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_actionOpen_triggered();

	void on_actionNormal_View_triggered();

	void on_actionLight_View_triggered();

	void on_actionScene_View_triggered();

	void on_actionWireframe_triggered();

	void on_actionShow_Mesh_triggered(bool checked);

	void on_actionShow_Skelton_triggered(bool checked);

private:
	void on_actionSetMode(int mode);

	void openModel(MODEL * model);

	void cleanupModel();

	void showMaterialEditor(int i);

private:
	Ui::MainWindow *ui;
	static QOpenGLWidget * con;

public:
	static void makeCurrent() { con->makeCurrent(); }
	static void doneCurrent() { con->doneCurrent(); }
};

#endif // MAINWINDOW_HPP
