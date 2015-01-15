#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "Global.h"
#include <QTableWidgetItem>

class TableManager {
protected:
	QTableWidgetItem* newItem(string text);
	QTableWidgetItem* newReadOnlyItem(string text);
	QTableWidgetItem* newCheckBoxItem(bool checked);
};

#endif // TABLE_MANAGER_H