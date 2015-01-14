#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include "Global.h"
#include <QTableWidget>

class AccountManager : public QObject {
	Q_OBJECT

public:
	AccountManager(Db db, QTableWidget* table, QObject* parent = 0);

public slots:
	void reload();

private:
	Db db;
	QTableWidget* table;
	std::vector<int> row2Id;
};

#endif // ACCOUNT_MANAGER_H