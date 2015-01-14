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

const char* DbPath = "db.sqlite";
const char* StatementFolder = "/Users/marvin/Workspace/Moneyphant/statements/";

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

	accountManager = new AccountManager(db, ui->accountTable, this);
	accountManager->reload();

	transferManager = new TransferManager(db, ui->transferTable, this);
	transferManager->reload();
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::openDb() {
	if (false) {
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

