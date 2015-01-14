#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Evolutions.h"
#include "db.h"
#include "Account.h"
#include "Transfer.h"
#include "StatementReader.h"

#include <iostream>

#include <QDir>
#include <QStringList>
#include <QDateTime>
#include <QColor>

const char* DbPath = "db.sqlite";
const char* StatementFolder = "/Users/marvin/Workspace/Moneyphant/statements/";

const QColor Green(80, 160, 40);
const QColor Red(240, 0, 0);

SQLPP_ALIAS_PROVIDER(fromName);
SQLPP_ALIAS_PROVIDER(toName);

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	dbConfig(nullptr),
	db(nullptr)
{
	ui->setupUi(this);

	openDb();
	Evolutions(db).run();

	StatementReader reader(db);
	reader.importMissingStatementFiles(StatementFolder);

	db::Account acc;
	int accCount = db->run(select(count(acc.id)).from(acc).where(true)).front().count;

	ui->accountTable->setRowCount(accCount);
	ui->accountTable->setColumnCount(8);

	ui->accountTable->setHorizontalHeaderLabels(QStringList{QString("Is Own"), QString("Name"), QString("Owner"), QString("IBAN"), QString("BIC"), QString("Account Number"), QString("Bank Code"), QString("Tags")});

	int row = 0;
	for (const auto& a : db->run(select(all_of(acc)).from(acc).where(true))) {
		ui->accountTable->setItem(row, 1, new QTableWidgetItem(qstr(a.name)));
		ui->accountTable->setItem(row, 2, new QTableWidgetItem(qstr(a.owner)));
		ui->accountTable->setItem(row, 3, new QTableWidgetItem(qstr(a.iban)));
		ui->accountTable->setItem(row, 4, new QTableWidgetItem(qstr(a.bic)));
		ui->accountTable->setItem(row, 5, new QTableWidgetItem(qstr(a.accountNumber)));
		ui->accountTable->setItem(row, 6, new QTableWidgetItem(qstr(a.bankCode)));

		row++;
	}


	db::Transfer tr;
	auto accFrom = acc.as(fromName);
	auto accTo = acc.as(toName);


	int trCount = db->run(select(count(tr.id)).from(tr).where(true)).front().count;

	ui->transferTable->setRowCount(trCount);
	ui->transferTable->setColumnCount(7);

	ui->transferTable->setHorizontalHeaderLabels(QStringList{QString("Date"), QString("From"), QString("To"), QString("Reference"), QString("Amount"), QString("Checked"), QString("Tags")});

	row = 0;
	for (const auto& t : db->run(select(all_of(tr), accFrom.name.as(fromName), accTo.name.as(toName)).from(tr, accFrom, accTo).
									where(tr.fromId == accFrom.id and tr.toId == accTo.id))) {
		ui->transferTable->setItem(row, 0, new QTableWidgetItem(QDateTime::fromMSecsSinceEpoch(t.date).toString("dd.MM.yyyy")));
		ui->transferTable->setItem(row, 1, new QTableWidgetItem(qstr(t.fromName)));
		ui->transferTable->setItem(row, 2, new QTableWidgetItem(qstr(t.toName)));
		ui->transferTable->setItem(row, 3, new QTableWidgetItem(qstr(t.reference)));
		ui->transferTable->setItem(row, 4, new QTableWidgetItem(QString("%1 €").arg(t.amount / 100.0, 0, 'f', 2)));
		ui->transferTable->item(row, 4)->setForeground(t.amount > 0 ? Green : Red);

		row++;
	}

}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::openDb() {
	if (true) {
		QFile dbFile(DbPath);
		if (dbFile.exists()) {
			bool b = dbFile.remove();
			assert(b && "Could not remove old database file");
		}
	}

	dbConfig = std::make_shared<sqlpp::sqlite3::connection_config>();
	assert(dbConfig != nullptr);
	dbConfig->path_to_database = str(DbPath);
	dbConfig->flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	dbConfig->debug = false;

	db = std::make_shared<sqlpp::sqlite3::connection>(dbConfig);
}

