#include "bonetreemodel.hpp"

struct BoneRef
{
	int index = -1;
	QModelIndex mindex;
	BoneRef * parent = nullptr;
	QVector<BoneRef*> children;

	explicit BoneRef() : index(-1), parent(nullptr), children()
	{

	}

	explicit BoneRef(BoneRef * parent, int index) : index(index), parent(parent), children()
	{
		parent->children.push_back(this);
	}

	void print(QString const & indent = "")
	{
		engine_log("%s%d", indent.toUtf8().data(), this->index);
		for(BoneRef * child : this->children)
		{
			child->print(indent + "\t");
		}
	}
};

BoneTreeModel::BoneTreeModel(MODEL * model, QObject *parent)
    : QAbstractItemModel(parent),
      model(model)
{
	Q_ASSERT(model != nullptr);

	myroot = new BoneRef();
	for(int i = 0; i < model->boneCount; i++)
	{
		BoneRef * parent;
		if(i == 0)
			parent = myroot;
		else
			parent = backrefs[model->bones[i].parent];
		backrefs[i] = new BoneRef(parent, i);
	}
	myroot->print();

	myroot->mindex = this->createIndex(0, 0, this->myroot);
	for(int i = 0; i < model->boneCount; i++)
	{
		backrefs[i]->mindex = this->createIndex(
			backrefs[i]->parent->children.indexOf(backrefs[i]), 0,
			backrefs[i]);
	}
}

BoneTreeModel::~BoneTreeModel()
{
	for(int i = 0; i < model->boneCount; i++) {
		delete backrefs[i];
	}
	delete myroot;
}

int BoneTreeModel::boneIndex(QModelIndex const & index) const
{
	if(index.isValid()) {
		auto ref = (BoneRef*)index.internalPointer();
		return ref->index;
	} else {
		return -1;
	}
}

QModelIndex BoneTreeModel::root()
{
	return this->myroot->mindex;
}

QVariant BoneTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		return QVariant("Bone");
	}
	return QVariant();
}

QModelIndex BoneTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(column);
	if(parent.isValid()) {
		auto ref = (BoneRef*)parent.internalPointer();
		Q_ASSERT(row >= 0 && row < ref->children.size());
		return ref->children[row]->mindex;
	} else {
		// "Having no parents must mean the node is the root of all others"
		// – Cpt. Obvious:
		return this->myroot->mindex;
	}
}

QModelIndex BoneTreeModel::parent(const QModelIndex &index) const
{
	if(!index.isValid()) {
		// Having no valid index means having no valid parent...
		return QModelIndex();
	}
	auto ref = (BoneRef*)index.internalPointer();
	if(ref->parent) {
		return ref->parent->mindex;
	} else {
		// Having no parent is sad, but that's just the way it is
		return QModelIndex();
	}
}

int BoneTreeModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid())
		return 0;
	auto ref = (BoneRef*)parent.internalPointer();
	return ref->children.size();
}

int BoneTreeModel::columnCount(const QModelIndex &parent) const
{
	if (!parent.isValid())
		return 0;
	return 1;
}

QVariant BoneTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	if (role == Qt::DisplayRole)
    {
		auto ref = (BoneRef*)index.internalPointer();
		return QString(model->bones[ref->index].name);
    }
    return QVariant();
}

void BoneTreeModel::refreshBone(int bone)
{
	emit this->dataChanged(
		backrefs[bone]->mindex,
		backrefs[bone]->mindex,
		{ Qt::DisplayRole });
}
