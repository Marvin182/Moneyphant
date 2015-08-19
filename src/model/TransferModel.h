#ifndef TRANSFER_MODEL_H
#define TRANSFER_MODEL_H

#include <vector>
#include <unordered_map>
#include <QAbstractTableModel>
#include "../sql.h"
#include "Transfer.h"

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

	bool empty() const { return cachedTransfers.empty(); }

	const Transfer& get(int row) const;
	const Transfer& getById(int id) const;

	void setNote(int transferId, cqstring note);
	void toggleChecked(const std::vector<int>& transferIds);
	void toggleInternal(const std::vector<int>& transferIds);
	void remove(const std::vector<int>& transferIds);

	void exportTransfers(cqstring path, const std::vector<int>& transferIds) const;

public slots:
	void invalidateCache();
	void onAccountsMerged(const Account& merged, QSet<int> mergedIds);
	void createBackup(const QString& path);

private:
	Db db;

	std::unordered_map<int, int> id2Row;
	std::vector<Transfer> cachedTransfers;
	
	Transfer& _get(int row);
	Transfer& _getById(int id);

	void save(const Transfer& t);
	void emitChanged(int id);

	void assertValidIndex(const QModelIndex& index) const;
};

#endif // TRANSFER_MODEL_H
