#include "TransferManager.h"
#include "db.h"
#include <QStringList>

#include <QDateTime>
#include <QColor>

const QColor Green(80, 160, 40);
const QColor Red(240, 0, 0);

SQLPP_ALIAS_PROVIDER(fromName)
SQLPP_ALIAS_PROVIDER(toName)

TransferManager::TransferManager(Db db, QTableWidget* table, QObject* parent) :
	QObject(parent),
	db(db),
	table(table)
{
	assert(table != nullptr);

	table->clear();
	table->setColumnCount(7);
	table->setHorizontalHeaderLabels(QStringList{tr("Date"), tr("From"), tr("To"), tr("Reference"), tr("Amount"), tr("Checked"), tr("Tags")});
}

void TransferManager::reload() {
	db::Account acc;
	db::Transfer tr;
	auto accFrom = acc.as(fromName);
	auto accTo = acc.as(toName);

	int rowCount = db->run(select(count(tr.id)).from(tr).where(true)).front().count;

	table->clearContents();
	row2Id.assign(rowCount, -1);
	table->setRowCount(rowCount);

	int row = 0;
	for (const auto& t : db->run(select(all_of(tr), accFrom.name.as(fromName), accTo.name.as(toName)).from(tr, accFrom, accTo).
									where(tr.fromId == accFrom.id and tr.toId == accTo.id))) {
		row2Id[row] = t.id;

		table->setItem(row, 0, new QTableWidgetItem(QDateTime::fromMSecsSinceEpoch(t.date).toString("dd.MM.yyyy")));
		table->setItem(row, 1, new QTableWidgetItem(qstr(t.fromName)));
		table->setItem(row, 2, new QTableWidgetItem(qstr(t.toName)));
		table->setItem(row, 3, new QTableWidgetItem(qstr(t.reference)));
		table->setItem(row, 4, new QTableWidgetItem(QString("%1 €").arg(t.amount / 100.0, 0, 'f', 2)));
		
		table->item(row, 4)->setForeground(t.amount > 0 ? Green : Red);
		auto item = new QTableWidgetItem(QTableWidgetItem::UserType);
		item->setCheckState(t.checked ? Qt::Checked : Qt::Unchecked);
		table->setItem(row, 5, item);

		row++;
	}
}
