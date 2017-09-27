#ifndef MODELANIMATIONLISTMODEL_HPP
#define MODELANIMATIONLISTMODEL_HPP

#include <acknext.h>
#include <QAbstractListModel>

class ModelAnimationListModel : public QAbstractListModel
{
	Q_OBJECT
private:
	MODEL * model;
public:
	explicit ModelAnimationListModel(MODEL * model, QObject *parent = nullptr);

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	ANIMATION * get(QModelIndex index) const {
		return this->model->animations[index.row()];
	}

private:
};

#endif // MODELANIMATIONLISTMODEL_HPP
