#include "modelanimationlistmodel.hpp"
#include <algorithm>

ModelAnimationListModel::ModelAnimationListModel(MODEL * model, QObject *parent) :
    QAbstractListModel(parent),
    model(model)
{
}

QVariant ModelAnimationListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	if(role == Qt::DisplayRole) {
		return QVariant("Animation");
	}
	return QVariant();
}

int ModelAnimationListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return this->model->animationCount;
}

QVariant ModelAnimationListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	if(role == Qt::DisplayRole) {
		int i = index.row();
		assert(i >= 0 && i < this->model->animationCount);
		return QVariant(this->model->animations[i]->name);
	}
	return QVariant();
}

void ModelAnimationListModel::refreshAnimation(ANIMATION * anim)
{
	int idx = -1;
	for(int i = 0; i < model->animationCount; i++) {
		if(model->animations[i] == anim) idx = i;
	}
	if(idx < 0)
		return;

	emit this->dataChanged(
		this->index(idx),
		this->index(idx),
		{ Qt::DisplayRole });
}
