#ifndef TRANSFER_MODEL_H
#define TRANSFER_MODEL_H

#include "Global.h"
#include "Transfer.h"
#include <vector>
#include <unordered_map>
#include <QAbstractTableModel>

class TransferModel : public QAbstractTableModel {
	Q_OBJECT

public:
	TransferModel(Db db, QObject* parent = 0);

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	Qt::ItemFlags flags(const QModelIndex& index) const;

	const Transfer& operator[](int id) const;
	const Transfer& get(int row) const;
	
public slots:
	void reloadCache();
	void createBackup(const QString& path);

private:
	Db db;

	std::unordered_map<int, int> id2Row;
	std::vector<Transfer> cachedTransfers;

	Transfer& transferById(int id);
};

#endif // TRANSFER_MODEL_H