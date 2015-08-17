#ifndef ACCOUNT_MODEL_H
#define ACCOUNT_MODEL_H

#include <vector>
#include <unordered_map>
#include <QSet>
#include <QAbstractTableModel>
#include "../sql.h"
#include "Account.h"

class AccountModel : public QAbstractTableModel {
	Q_OBJECT

public:
	AccountModel(Db db, QObject* parent = 0);

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	Qt::ItemFlags flags(const QModelIndex& index) const;

	const Account& get(int row) const;
	const Account& getById(int id) const;

	static QSet<int> ownAccountIds(Db db);

public slots:
	void invalidateCache();
	void createBackup(const QString& path);
	void setInitialBalance(int id, int value);
	void mergeAccounts(int firstId, int secondId);

signals:
	void accountsMerged(const Account& merged, QSet<int> mergedIds);

private:
	Db db;

	std::unordered_map<int, int> id2Row;
	std::vector<Account> cachedAccounts;

	Account& _get(int row);
	Account& _getById(int id);
	void save(const Account& a);

	void assertValidIndex(const QModelIndex& index) const;
};

#endif // ACCOUNT_MODEL_H
