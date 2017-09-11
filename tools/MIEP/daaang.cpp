#include "daaang.hpp"

#include <QCoreApplication>
#include <QDebug>

Daaang::Daaang(QWidget *parent) : QOpenGLWidget(parent)
{

	this->setMinimumSize(100, 100);
}

void Daaang::initializeGL()
{


	emit done();
}
