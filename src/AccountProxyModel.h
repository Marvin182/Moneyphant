#ifndef ACCOUNT_PROXY_MODEL_H
#define ACCOUNT_PROXY_MODEL_H

#include <vector>
#include <QDateTime>
#include <mr/common>
#include "sql.h"
#include "Account.h"
#include "QSortFilterProxyModel.h"

class AccountProxyModel : public QSortFilterProxyModel {
	Q_OBJECT

public:
	AccountProxyModel(Db db, QObject* parent = 0);

public slots:
	void setFilterText(const QString& filterText);

signals:

protected:
	Db db;

	int accIsOwn;
	int accIsSpecial;
	QString txtName;
	QString txtOwner;
	std::vector<int> txtTags;
	QString txtRest;

	const Account& get(int sourceRow) const;

	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
	bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
};

#endif // ACCOUNT_PROXY_MODEL_H
