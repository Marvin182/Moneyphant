#include "AccountManager.h"
#include "db.h"
#include <QStringList>

AccountManager::AccountManager(Db db, QTableWidget* table, QObject* parent) :
	QObject(parent),
	db(db),
	table(table)
{
	assert(table != nullptr);

	table->clear();
	table->setColumnCount(8);
	table->setHorizontalHeaderLabels(QStringList{tr("Is Own"), tr("Name"), tr("Owner"), tr("IBAN"), tr("BIC"), tr("Account Number"), tr("Bank Code"), tr("Tags")});
}

void AccountManager::reload() {
	db::Account acc;
	int rowCount = db->run(select(count(acc.id)).from(acc).where(true)).front().count;

	table->clearContents();
	row2Id.assign(rowCount, -1);
	table->setRowCount(rowCount);

	int row = 0;
	for (const auto& a : db->run(select(all_of(acc)).from(acc).where(true))) {
		row2Id[row] = a.id;
		
		auto isOwnItem = new QTableWidgetItem(QTableWidgetItem::UserType);
		isOwnItem->setCheckState(a.isOwn ? Qt::Checked : Qt::Unchecked);

		table->setItem(row, 0, isOwnItem);
		table->setItem(row, 1, new QTableWidgetItem(qstr(a.name)));
		table->setItem(row, 2, new QTableWidgetItem(qstr(a.owner)));
		table->setItem(row, 3, new QTableWidgetItem(qstr(a.iban)));
		table->setItem(row, 4, new QTableWidgetItem(qstr(a.bic)));
		table->setItem(row, 5, new QTableWidgetItem(qstr(a.accountNumber)));
		table->setItem(row, 6, new QTableWidgetItem(qstr(a.bankCode)));
		table->setItem(row, 6, new QTableWidgetItem(tr("")));

		row++;
	}
}
