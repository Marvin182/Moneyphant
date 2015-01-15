#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "Evolutions.h"
#include "db.h"
#include "Transfer.h"
#include "StatementReader.h"
#include "TagLabel.h"

#include <iostream>

#include <QDir>
#include <QStringList>
#include <QDateTime>
#include <QSortFilterProxyModel>
#include <QLabel>

const char* DbPath = "db.sqlite";
const char* StatementFolder = "/Users/marvin/Workspace/Moneyphant/statements/";

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	dbConfig(nullptr),
	db(nullptr),
	currentAccountId(-1),
	accountModel(nullptr)
{
	ui->setupUi(this);

	openDb();
	Evolutions(db).run();

	StatementReader reader(db);
	reader.importMissingStatementFiles(StatementFolder);

	setupAccountTab();
	setupTransferTab();
}

void MainWindow::onAccountEntered(const QModelIndex& index) {
	assert(accountModel != nullptr);
	const auto& a = accountModel->get(index.row());
	assert(a.id >= 0);
	currentAccountId = a.id;

	while (!ui->accountTags->layout()->isEmpty()) {
		delete ui->accountTags->layout()->itemAt(0)->widget();
	}

	db::Tag tag;
	for (const auto& t : db->run(select(tag.accountId, tag.name).from(tag).where(tag.accountId == a.id))) {
		auto tagLabel = new TagLabel(Tag(t.accountId, -1, qstr(t.name)), this);
		ui->accountTags->layout()->addWidget(tagLabel);
	}
}

void MainWindow::addAccountTag() {
	if (currentAccountId == -1) {
		return;
	}
	assert(currentAccountId >= 0);

	const Account& a = (*accountModel)[currentAccountId];
	auto name = ui->addAccountTag->text();

	db::Tag tag;
	bool exists = db->run(select(count(tag.name)).from(tag).where(tag.accountId == a.id and tag.name == str(name))).front().count > 0;
	if (exists) {
		return;
	}
	db->run(insert_into(tag).set(tag.accountId = a.id, tag.name = str(name)));

	auto tagLabel = new TagLabel(Tag(a.id, -1, name), this);
	connect(tagLabel, SIGNAL(deleteClicked(Tag)), this, SLOT(removeAccountTag(Tag)));
	ui->accountTags->layout()->addWidget(tagLabel);

	ui->addAccountTag->clear();
}

void MainWindow::removeAccountTag(Tag t) {
	std::cout << "remove " << t << std::endl;
}

void MainWindow::setupAccountTab() {
	accountModel = new AccountModel(db, this);

	// setup proxy model for sorting and filtering
	auto accountProxyModel = new QSortFilterProxyModel(this);
	accountProxyModel->setSourceModel(accountModel);
	accountProxyModel->setFilterKeyColumn(1);
	accountProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	accountProxyModel->setFilterRole(Qt::UserRole + 1);

	ui->accountView->setModel(accountProxyModel);
	ui->accountView->setSortingEnabled(true);

	// mouse tracking for hover
	ui->accountView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->accountView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	ui->accountView->setMouseTracking(true);
	connect(ui->accountView, SIGNAL(entered(const QModelIndex&)), this, SLOT(onAccountEntered(const QModelIndex&)));

	// configure headers
	ui->accountView->verticalHeader()->hide();
	ui->accountView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->accountView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	ui->accountView->horizontalHeader()->setDefaultSectionSize(60);
	ui->accountView->horizontalHeader()->hideSection(7);
	
	connect(ui->accountSearch, SIGNAL(textChanged(const QString&)), accountProxyModel, SLOT(setFilterWildcard(const QString&)));
	connect(ui->addAccountTag, SIGNAL(returnPressed()), this, SLOT(addAccountTag()));
}

void MainWindow::setupTransferTab() {
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

