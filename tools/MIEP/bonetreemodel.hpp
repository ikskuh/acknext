#ifndef BONETREEMODEL_HPP
#define BONETREEMODEL_HPP

#include <acknext.h>
#include <QAbstractItemModel>
#include <QVector>
#include <memory>

struct BoneRef;

class BoneTreeModel : public QAbstractItemModel
{
	Q_OBJECT
private:
	MODEL * model;
	BoneRef * myroot;
	BoneRef * backrefs[ACKNEXT_MAX_BONES];
public:
	explicit BoneTreeModel(MODEL * model, QObject *parent = nullptr);
	BoneTreeModel(BoneTreeModel const &) = delete;
	BoneTreeModel(BoneTreeModel &&) = delete;
	~BoneTreeModel();

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Basic functionality:
	QModelIndex index(int row, int column,
	                  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	QModelIndex root();

	int boneIndex(QModelIndex const & index) const;

	void refreshBone(int bone);
};

#endif // BONETREEMODEL_HPP
