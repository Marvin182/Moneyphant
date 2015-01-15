#include "TableManager.h"

QTableWidgetItem* TableManager::newItem(string text) {
	auto item = new QTableWidgetItem(text);
	item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
	return item;
}

QTableWidgetItem* TableManager::newReadOnlyItem(string text) {
	auto item = new QTableWidgetItem(text);
	item->setFlags(Qt::NoItemFlags);
	return item;
}

QTableWidgetItem* TableManager::newCheckBoxItem(bool checked) {
	auto item = new QTableWidgetItem(QTableWidgetItem::UserType);
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
	item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
	return item;
}
