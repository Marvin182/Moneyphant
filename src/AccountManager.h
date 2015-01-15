#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include "Global.h"
#include <QTableWidget>
#include "TableManager.h"
#include "db.h"

class AccountManager : public QObject, public TableManager {
	Q_OBJECT

public:
	AccountManager(Db db, QTableWidget* table, QObject* parent = 0);

public slots:
	void reload();

private slots:
	void onCellChanged(int row, int column);

private:
	Db db;
	QTableWidget* table;
	std::vector<int> row2Id;
};

#endif // ACCOUNT_MANAGER_H