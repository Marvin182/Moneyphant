#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "Evolutions.h"
#include "db.h"
#include "Transfer.h"
#include "StatementReader.h"
#include <iostream>
#include <QDir>
#include <QStringList>
#include <QDateTime>

const char* DbPath = "db.sqlite";
const char* StatementFolder = "/Users/marvin/Workspace/Moneyphant/statements/";
const char* BackupFolder = "/Users/marvin/Workspace/Moneyphant/backups";

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
	tagHelper = new TagHelper(db, this);

	StatementReader reader(db);
	reader.importMissingStatementFiles(StatementFolder);

	setupAccountTab();
	setupTransferTab();
}

void MainWindow::setCurrentAccount(const QModelIndex& idx) {
	assert(accountModel != nullptr);
	assert(accountProxyModel != nullptr);

	const auto& index = accountProxyModel->mapToSource(idx);
	int id = accountModel->get(index.row()).id;
	if (currentAccountId == id) {
		return;
	}
	currentAccountId = id;

	auto selectionModel = ui->accountView->selectionModel();
	if (!selectionModel->hasSelection()) {
		tagHelper->setAccountIds({currentAccountId});
	}

	updateAccountInfo();
}

void MainWindow::showSelectedAccounts(const QItemSelection& selected, const QItemSelection& deseceted) {
	for (const auto& idx : deseceted.indexes()) {
		const auto& index = accountProxyModel->mapToSource(idx);
		if (index.column() != 0) {
			continue;
		}
		int id = accountModel->get(index.row()).id;
		tagHelper->removeAccountId(id);
	}

	for (const auto& idx : selected.indexes()) {
		const auto& index = accountProxyModel->mapToSource(idx);
		if (index.column() != 0) {
			continue;
		}
		int id = accountModel->get(index.row()).id;
		tagHelper->addAccountId(id);
	}

	updateAccountInfo();
}
	
void MainWindow::updateAccountInfo() {
	assert(currentAccountId >= 0);

	const auto& ids = tagHelper->accountIds();
	auto selectionModel = ui->accountView->selectionModel();

	// ui->accountName->setEnabled(ids.size() == 1);
	ui->accountTags->setEnabled(selectionModel->hasSelection());
	ui->moreAccountTags->setText("");
	ui->mergeAccounts->setVisible(ids.size() == 2);
	ui->actionMerge_Selected_Accounts->setEnabled(ids.size() == 2);

	if (ids.empty()) {
		return;
	} else if (ids.size() == 1) {
		updateAccountDetails((*accountModel)[ids.front()]);
		ui->accountTags->setTags(tagHelper->getAccountTags(), tagHelper->accountIds());
	} else {
		const auto& currentAccount = (*accountModel)[currentAccountId];
		updateAccountDetails(currentAccount);

		const auto& commonTags = tagHelper->getAccountTags();
		ui->accountTags->setTags(commonTags, tagHelper->accountIds());

		const auto& currentAccountTags = tagHelper->getAccountTags({currentAccountId});
		QStringList moreTags;
		for (const auto& t : currentAccountTags) {
			if (!commonTags.contains(t)) {
				moreTags << t;
			}
		}
		ui->moreAccountTags->setText(moreTags.join("; "));
	}
}

void MainWindow::updateAccountDetails(const Account& account) {
	ui->accountName->setText(account.name);
	ui->accountOwner->setText(account.owner);
	ui->accountIban->setText(account.iban);
	ui->accountBic->setText(account.bic);
	ui->accountAccountNumber->setText(account.accountNumber);
	ui->accountBankCode->setText(account.bankCode);
}

void MainWindow::mergeAccounts() {
	assert(tagHelper->accountIds().size() == 2);
	accountModel->mergeAccounts(tagHelper->accountIds().front(), tagHelper->accountIds().back());
	ui->accountView->selectionModel()->clearSelection();
	tagHelper->setAccountIds({});
}

void MainWindow::setupAccountTab() {
	accountModel = new AccountModel(db, this);

	// setup proxy model for sorting and filtering
	accountProxyModel = new QSortFilterProxyModel(this);
	accountProxyModel->setSourceModel(accountModel);
	accountProxyModel->setFilterKeyColumn(1);
	accountProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	accountProxyModel->setFilterRole(Qt::UserRole + 1);

	ui->accountView->setModel(accountProxyModel);
	// ui->accountView->setSortingEnabled(true);

	// mouse tracking for hover
	ui->accountView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->accountView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	ui->accountView->setMouseTracking(true);
	connect(ui->accountView, SIGNAL(entered(const QModelIndex&)), this, SLOT(setCurrentAccount(const QModelIndex&)));
	connect(ui->accountView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(showSelectedAccounts(const QItemSelection&, const QItemSelection&)));
	connect(ui->accountTags, SIGNAL(tagsAdded(QStringList, const std::vector<int>&)), tagHelper, SLOT(addAccountTags(QStringList, const std::vector<int>&)));
	connect(ui->accountTags, SIGNAL(tagsRemoved(QStringList, const std::vector<int>&)), tagHelper, SLOT(removeAccountTags(QStringList, const std::vector<int>&)));
	connect(ui->actionMerge_Selected_Accounts, SIGNAL(triggered()), this, SLOT(mergeAccounts()));
	connect(ui->mergeAccounts, SIGNAL(clicked()), this, SLOT(mergeAccounts()));

	ui->mergeAccounts->setVisible(false);

	// configure headers
	ui->accountView->verticalHeader()->hide();
	ui->accountView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->accountView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	ui->accountView->horizontalHeader()->setDefaultSectionSize(60);
	ui->accountView->horizontalHeader()->hideSection(7);
	
	connect(ui->accountSearch, SIGNAL(textChanged(const QString&)), accountProxyModel, SLOT(setFilterWildcard(const QString&)));
}

void MainWindow::setupTransferTab() {
	transferManager = new TransferManager(db, ui->transferTable, this);
	transferManager->reload();
}

MainWindow::~MainWindow() {
	auto now = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");

	assert(accountModel != nullptr);
	accountModel->createBackup(QString("%1/%2.accounts.csv").arg(BackupFolder).arg(now));

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

