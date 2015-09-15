#ifndef TAG_MODEL_H
#define TAG_MODEL_H

#include <QAbstractListModel>
#include "../sql.h"

class TagModel : public QAbstractListModel {
	Q_OBJECT
public:
	TagModel(Db db, QObject* parent = 0);

	int rowCount(const QModelIndex& parent = QModelIndex()) const;

	QHash<int, QByteArray> roleNames() const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
	Db db;
};

#endif // TAG_MODEL_H
