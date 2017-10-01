#include "meshlistmodel.hpp"

MeshListModel::MeshListModel(MODEL * model, QObject *parent)
    : QAbstractListModel(parent),
      model(model)
{

}

QVariant MeshListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	if(role == Qt::DisplayRole) {
		return QVariant("Mesh");
	}
	return QVariant();
}

int MeshListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return model->meshCount;
}

QVariant MeshListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	if(role == Qt::DisplayRole) {
		return QVariant("Mesh " + QString::number(index.row()));
	}
	return QVariant();
}
