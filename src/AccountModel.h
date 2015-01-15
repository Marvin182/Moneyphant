#ifndef ACCOUNT_MODEL_H
#define ACCOUNT_MODEL_H

#include "Global.h"
#include "Account.h"
#include <vector>
#include <unordered_map>
#include <QAbstractTableModel>

class AccountModel : public QAbstractTableModel {
	Q_OBJECT

public:
	AccountModel(Db db, QObject* parent = 0);

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool setData(const QModelIndex& index, const QVariant & value, int role = Qt::EditRole);
	Qt::ItemFlags flags(const QModelIndex& index) const;

	const Account& operator[](int id) const;
	const Account& get(int row) const;

public slots:
	void reloadCache();

private:
	Db db;

	std::unordered_map<int, int> id2Row;
	std::vector<Account> cachedAccounts;
};

#endif // ACCOUNT_MODEL_H
