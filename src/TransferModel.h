#ifndef TRANSFER_MODEL_H
#define TRANSFER_MODEL_H

#include "globals/all.h"
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

	Transfer& get(int row);
	const Transfer& get(int row) const;
	Transfer& getById(int id);
	const Transfer& getById(int id) const;

	void setNote(int transferId, cqstring note);
	void setChecked(const std::vector<int>& transferIds, bool checked);
	void exportTransfers(cqstring path, const std::vector<int>& transferIds) const;

public slots:
	void reloadCache();
	void createBackup(const QString& path);

private:
	Db db;

	std::unordered_map<int, int> id2Row;
	std::vector<Transfer> cachedTransfers;

	template <typename F>
	void updateTransfer(int transferId, F f);
	void assertValidIndex(const QModelIndex& index) const;
};

#endif // TRANSFER_MODEL_H
