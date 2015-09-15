#include "TagModel.h"

#include <mr/common>

TagModel::TagModel(Db db, QObject* parent) :
	QAbstractListModel(parent),
	db(db)
{}

int TagModel::rowCount(const QModelIndex& parent) const {
	db::Tag tg;
	auto r = (*db)(select(count(tg.id)).from(tg).where(true));
	return r.front().count;
}

QVariant TagModel::data(const QModelIndex& index, int role) const {
	assert_error(index.row() >= 0 && index.row() < rowCount(), "index.row() = %d, rowCount() = %d", index.row(), rowCount());
	assert_error(index.column() == 0);
	db::Tag tg;
	auto r = (*db)(select(tg.name).from(tg).where(true).limit(1).offset(index.row()));
	assert_error(!r.empty());
	switch (role) {
		case Qt::DisplayRole:
			return qstr(r.front().name);
			break;
		default:
			return QVariant();
	}
}

QHash<int, QByteArray> TagModel::roleNames() const {
	QHash<int, QByteArray> roles;
	roles[Qt::DisplayRole] = "display";
	return roles;
}

QVariant TagModel::headerData(int section, Qt::Orientation orientation, int role) const {
	assert_error(section == 0);
	assert_error(role == Qt::DisplayRole);
	return "Name";
}
