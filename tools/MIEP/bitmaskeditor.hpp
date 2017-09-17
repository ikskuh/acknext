#ifndef BITMASKEDITOR_HPP
#define BITMASKEDITOR_HPP

#include <acknext.h>
#include <QWidget>

namespace Ui {
	class BitMaskEditor;
}

class BitMaskEditor : public QWidget
{
	Q_OBJECT
private:
	MESH * mesh;
public:
	explicit BitMaskEditor(QWidget *parent = 0);
	~BitMaskEditor();

	void setMesh(MESH * mesh);

signals:
	Q_SIGNAL void hasChanged();

private slots:
	void on_maximum_valueChanged(int value);

	void on_minimum_valueChanged(int value);

	void on_checkBox_stateChanged(int arg1);

private:
	void updateMesh();
private:
	Ui::BitMaskEditor *ui;
};

#endif // BITMASKEDITOR_HPP
