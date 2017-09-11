#ifndef MESHLISTMODEL_HPP
#define MESHLISTMODEL_HPP

#include <acknext.h>
#include <QAbstractListModel>

class MeshListModel : public QAbstractListModel
{
	Q_OBJECT
	MODEL * model;
public:
	explicit MeshListModel(MODEL * model, QObject *parent = nullptr);

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // MESHLISTMODEL_HPP
