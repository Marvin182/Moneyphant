#ifndef TRANSFER_MANAGER_H
#define TRANSFER_MANAGER_H

#include "Global.h"
#include <QTableWidget>

class TransferManager : public QObject {
	Q_OBJECT

public:
	TransferManager(Db db, QTableWidget* table, QObject* parent = 0);

public slots:
	void reload();

private:
	Db db;
	QTableWidget* table;
	std::vector<int> row2Id;
};

#endif // TRANSFER_MANAGER_H