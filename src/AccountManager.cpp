#include "AccountManager.h"
#include "db.h"
#include <QStringList>

#include <iostream>

AccountManager::AccountManager(Db db, QTableWidget* table, QObject* parent) :
	QObject(parent),
	db(db),
	table(table)
{
	assert(table != nullptr);

	table->clear();
	table->setColumnCount(8);
	table->setHorizontalHeaderLabels(QStringList{tr("Is Own"), tr("Name"), tr("Owner"), tr("IBAN"), tr("BIC"), tr("Account Number"), tr("Bank Code"), tr("Tags")});

	connect(table, SIGNAL(cellChanged(int, int)), this, SLOT(onCellChanged(int, int)));
}

void AccountManager::onCellChanged(int row, int column) {
	assert(row >= 0 && row < (int) row2Id.size());
	int id = row2Id[row];
	assert(id >= 0);

	auto item = table->item(row, column);
	bool checked = item->checkState() == Qt::Checked;
	auto text = item->text();

	db::Account acc;

	switch (column) {
		case 0:
			db->run(update(acc).set(acc.isOwn = checked).where(acc.id == id));
			break;
		case 1:
			db->run(update(acc).set(acc.name = str(text)).where(acc.id == id));
			break;
		case 2:
			db->run(update(acc).set(acc.owner = str(text)).where(acc.id == id));
			break;
		case 3:
			db->run(update(acc).set(acc.iban = str(text)).where(acc.id == id));
			break;
		case 4:
			db->run(update(acc).set(acc.bic = str(text)).where(acc.id == id));
			break;
		case 5:
			db->run(update(acc).set(acc.accountNumber = str(text)).where(acc.id == id));
			break;
		case 6:
			db->run(update(acc).set(acc.bankCode = str(text)).where(acc.id == id));
			break;
	}
}

void AccountManager::reload() {
	disconnect(table, SIGNAL(cellChanged(int, int)), this, SLOT(onCellChanged(int, int)));

	db::Account acc;
	int rowCount = db->run(select(count(acc.id)).from(acc).where(true)).front().count;

	table->clearContents();
	row2Id.assign(rowCount, -1);
	table->setRowCount(rowCount);

	int row = 0;
	for (const auto& a : db->run(select(all_of(acc)).from(acc).where(true))) {
		row2Id[row] = a.id;
		
		table->setItem(row, 0, newCheckBoxItem(a.isOwn));
		table->setItem(row, 1, newItem(qstr(a.name)));
		table->setItem(row, 2, newItem(qstr(a.owner)));
		table->setItem(row, 3, newItem(qstr(a.iban)));
		table->setItem(row, 4, newItem(qstr(a.bic)));
		table->setItem(row, 5, newItem(qstr(a.accountNumber)));
		table->setItem(row, 6, newItem(qstr(a.bankCode)));
		table->setItem(row, 7, newItem(tr("")));

		row++;
	}

	connect(table, SIGNAL(cellChanged(int, int)), this, SLOT(onCellChanged(int, int)));
}
