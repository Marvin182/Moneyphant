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
#include <QLabel>

const char* DbPath = "db.sqlite";
const char* StatementFolder = "/Users/marvin/Workspace/Moneyphant/statements/";
const char* BackupFolder = "/Users/marvin/Workspace/Moneyphant/backups";

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	dbConfig(nullptr),
	db(nullptr),
	currentAccountId(-1),
	accountModel(nullptr),
	accountProxyModel(nullptr),
	currentTransferId(-1),
	transferModel(nullptr),
	transferProxyModel(nullptr)
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

MainWindow::~MainWindow() {
	auto now = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");

	assert(accountModel != nullptr);
	accountModel->createBackup(QString("%1/%2.accounts.csv").arg(BackupFolder).arg(now));
	
	assert(transferModel != nullptr);
	transferModel->createBackup(QString("%1/%2.transfers.csv").arg(BackupFolder).arg(now));

	delete ui;
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

void MainWindow::setCurrentTransfer(const QModelIndex& idx) {
	assert(transferModel != nullptr);
	assert(transferProxyModel != nullptr);

	const auto& index = transferProxyModel->mapToSource(idx);
	int id = transferModel->get(index.row()).id;

	// set current id
	if (currentTransferId == id) {
		return;
	}
	currentTransferId = id;

	// clear saved ids of we don't have a selection
	if (!ui->transferView->selectionModel()->hasSelection()) {
		tagHelper->setTransferIds({});
	}

	updateTransferTags();
}

void MainWindow::showSelectedTransfers(const QItemSelection& selected, const QItemSelection& deseceted) {
	for (const auto& idx : deseceted.indexes()) {
		const auto& index = transferProxyModel->mapToSource(idx);
		if (index.column() != 0) {
			continue;
		}
		int id = transferModel->get(index.row()).id;
		tagHelper->removeTransferId(id);
	}

	for (const auto& idx : selected.indexes()) {
		const auto& index = transferProxyModel->mapToSource(idx);
		if (index.column() != 0) {
			continue;
		}
		int id = transferModel->get(index.row()).id;
		tagHelper->addTransferId(id);
	}

	updateTransferTags();
}
	
void MainWindow::updateTransferTags() {
	assert(currentTransferId >= 0);

	const auto& selectedIds = tagHelper->transferIds();
	const auto& ids = selectedIds.empty() ? std::vector<int>(1, currentTransferId) : selectedIds;

	ui->transferTags->setTags(tagHelper->getTransferTags(ids), ids);




	// const auto& detailTransfer = (*transferModel)[selectedIds.size() == 1 ? selectedIds.front() : currentTransferId];




	// auto selectionModel = ui->accountView->selectionModel();

	// ui->transferTagsFromAccounts->setText("");
	// assert(currentAccountId != -1 || !ids.empty());
	
	// int id = ids.size() == 1 ? ids.front() : currentTransferId;
	// const auto& t = (*transferModel)[id];
	// auto accountTags = tagHelper->accountTagsOfTransfer(t.from.id, t.to.id);
	// ui->transferTagsFromAccounts->setText(accountTags.join("; "));

	// ui->accountName->setEnabled(ids.size() == 1);
	// ui->accountTags->setEnabled(selectionModel->hasSelection());
	// ui->moreAccountTags->setText("");

	// if (ids.empty()) {
	// 	return;
	// } else if (ids.size() == 1) {
	// 	updateAccountDetails((*transferModel)[ids.front()]);
	// 	ui->accountTags->setTags(tagHelper->getTransferTags(), tagHelper->accountIds());
	// } else {
	// 	const auto& currentAccount = (*transferModel)[currentTransferId];
	// 	updateAccountDetails(currentAccount);

	// 	const auto& commonTags = tagHelper->getTransferTags();
	// 	ui->accountTags->setTags(commonTags, tagHelper->accountIds());

	// 	const auto& currentAccountTags = tagHelper->getTransferTags({currentTransferId});
	// 	QStringList moreTags;
	// 	for (const auto& t : currentAccountTags) {
	// 		if (!commonTags.contains(t)) {
	// 			moreTags << t;
	// 		}
	// 	}
	// 	ui->moreAccountTags->setText(moreTags.join("; "));
	// }


	updateTransferDetails();
}

void MainWindow::updateTransferDetails() {
	const auto& selectedIds = tagHelper->transferIds();
	bool editEnabled = selectedIds.size() == 1;

	ui->transferFrom->setEnabled(editEnabled);
	ui->transferNote->setEnabled(editEnabled);

	const auto& transfer = (*transferModel)[editEnabled ? selectedIds[0] : currentTransferId];

	ui->transferFrom->setText(transfer.from.name);
	ui->transferReference->setText(transfer.reference);
	ui->transferNote->setText(transfer.note);
	ui->tagsFromAccounts->setText(tagHelper->tagsFromAccounts(transfer.from.id, transfer.to.id).join("; "));
}

void MainWindow::resetTransferStats() {
	transferStats.clear();
	addToTransferStats(-1);
}

void MainWindow::addToTransferStats(int transferId) {
	if (transferId >= 0) {
		const auto& transfer = (*transferModel)[transferId];
		const auto& from = (*accountModel)[transfer.from.id];
		const auto& to = (*accountModel)[transfer.to.id];
		transferStats.add(transfer, transfer.internal || (from.isOwn && to.isOwn));
	}

	ui->trStatsRevenues->setText(euro(transferStats.revenues()));
	ui->trStatsExpenses->setText(euro(transferStats.expenses()));
	ui->trStatsInternal->setText(euro(transferStats.internal()));
	ui->trStatsProfit->setText(euro(transferStats.profit()));
}

void MainWindow::removeFromTransferStats(int transferId) {
	assert(transferId >= 0);
	const auto& transfer = (*transferModel)[transferId];
	const auto& from = (*accountModel)[transfer.from.id];
	const auto& to = (*accountModel)[transfer.to.id];
	transferStats.remove(transfer, transfer.internal || (from.isOwn && to.isOwn));

	ui->trStatsRevenues->setText(euro(transferStats.revenues()));
	ui->trStatsExpenses->setText(euro(transferStats.expenses()));
	ui->trStatsInternal->setText(euro(transferStats.internal()));
	ui->trStatsProfit->setText(euro(transferStats.profit()));
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
	
	connect(ui->accountSearch, SIGNAL(textChanged(const QString&)), accountProxyModel, SLOT(setFilterWildcard(const QString&)));
}

void MainWindow::setupTransferTab() {

	// model and proxy model for filtering and sorting
	transferModel = new TransferModel(db, this);
	transferProxyModel = new TransferProxyModel(db, this);
	transferProxyModel->setSourceModel(transferModel);

	// filter edits
	// connect(ui->trFilterFromAccount, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTransferFilter(int)));
	connect(ui->trFilterStartDate, SIGNAL(dateTimeChanged(const QDateTime&)), transferProxyModel, SLOT(setStartDate(const QDateTime&)));
	connect(ui->trFilterEndDate, SIGNAL(dateTimeChanged(const QDateTime&)), transferProxyModel, SLOT(setEndDate(const QDateTime&)));
	connect(ui->trFilterText, SIGNAL(textChanged(const QString&)), transferProxyModel, SLOT(setFilterText(const QString&)));
	createTransferFilterMonthLinks();
	ui->trFilterText->setText("ruslangrinmiete");

	// stats
	connect(transferProxyModel, SIGNAL(resetStats()), this, SLOT(resetTransferStats()));
	connect(transferProxyModel, SIGNAL(addToStats(int)), this, SLOT(addToTransferStats(int)));
	connect(transferProxyModel, SIGNAL(removeFromStats(int)), this, SLOT(removeFromTransferStats(int)));

	// main view
	ui->transferView->setModel(transferProxyModel);
	ui->transferView->verticalHeader()->hide();
	ui->transferView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->transferView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // Date
	ui->transferView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed); // Amount
	ui->transferView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed); // Checked
	ui->transferView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed); // Internal
	ui->transferView->horizontalHeader()->setDefaultSectionSize(100);
	ui->transferView->horizontalHeader()->hideSection(1); // From Account

	// enable sorting
	ui->transferView->setSortingEnabled(true);
	ui->transferView->sortByColumn(0, Qt::AscendingOrder);

	// enable row selection
	ui->transferView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->transferView->setSelectionMode(QAbstractItemView::ExtendedSelection); // support for multi rows and Strg/Shift functionality

	ui->transferView->setMouseTracking(true);
	connect(ui->transferView, SIGNAL(entered(const QModelIndex&)), this, SLOT(setCurrentTransfer(const QModelIndex&)));
	connect(ui->transferView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(showSelectedTransfers(const QItemSelection&, const QItemSelection&)));
	connect(ui->transferTags, SIGNAL(tagsAdded(QStringList, const std::vector<int>&)), tagHelper, SLOT(addTransferTags(QStringList, const std::vector<int>&)));
	connect(ui->transferTags, SIGNAL(tagsRemoved(QStringList, const std::vector<int>&)), tagHelper, SLOT(removeTransferTags(QStringList, const std::vector<int>&)));
}

void MainWindow::clickedTransferFilterMonthLink() {
	auto senderButton = (QPushButton*)sender();
	auto text = senderButton->text();

	QDateTime start, end;
	if (text.length() == 4) {
		// year range
		int year = text.toInt();
		start = QDateTime(QDate(year, 1, 1));
		end = QDateTime(QDate(year, 12, 31), QTime(23, 59));
	} else {
		// month range
		start = QDateTime::fromString(senderButton->text(), "MMM yy");
		if (start.date().year() < 1970) {
			start = start.addYears(100);
		}
		end = start.addMonths(1).addSecs(-1);
	}
	ui->trFilterStartDate->setDateTime(start);	
	ui->trFilterEndDate->setDateTime(end);
}

void MainWindow::createTransferFilterMonthLinks() {
	if (transferModel->rowCount() == 0) {
		return;
	}

	db::Transfer tr;
	auto startEnd = db->run(select(min(tr.date), max(tr.date)).from(tr).where(true));
	auto start = QDateTime::fromMSecsSinceEpoch(startEnd.front().min).date();
	auto end = QDateTime::fromMSecsSinceEpoch(startEnd.front().max).date();

	auto addButton = [&](const QString& text) {
		auto button = new QPushButton(text);
		connect(button, SIGNAL(pressed()), this, SLOT(clickedTransferFilterMonthLink()));
		ui->trFilterMonthLinks->addWidget(button);
	};

	// years
	for (int y = start.year(); y <= end.year(); y++) {
		addButton(QString::number(y));
	}

	// months
	while (start <= end) {
		addButton(start.toString("MMM yy"));
		start = start.addMonths(1);
	}

	// last month if needed
	if (start.month() == end.month() && start.day() > end.day()) {
		addButton(start.toString("MMM yy"));
	}
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

