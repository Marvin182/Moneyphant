#ifndef ACCOUNT_MODEL_H
#define ACCOUNT_MODEL_H

#include <vector>
#include <unordered_map>
#include <QSet>
#include <QAbstractTableModel>
#include <mr/common>
#include "sql.h"
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
	void reloadCache();
	void createBackup(const QString& path);
	void mergeAccounts(int firstId, int secondId);

private:
	Db db;

	std::unordered_map<int, int> id2Row;
	std::vector<Account> cachedAccounts;

	Account& _get(int row);
	Account& _getById(int id);

	void assertValidIndex(const QModelIndex& index) const;
};

#endif // ACCOUNT_MODEL_H
