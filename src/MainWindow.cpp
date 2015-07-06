#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "Evolutions.h"
#include "sql.h"
#include "Transfer.h"
#include "StatementReader.h"
#include "ui/AboutDialog.h"
#include "ui/StatementImporterDialog.h"
#include <iostream>
#include <QDir>
#include <QStringList>
#include <QDateTime>
#include <QLabel>
#include <QTimer>
#include <QFileDialog>

#include <QFileInfo>
#include <QRegExp>

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

	mr::Version::get(GIT_VERSION);

	loadSettings();
	initAssertHandler();

	QTimer::singleShot(0, this, SLOT(init()));
}

MainWindow::~MainWindow() {
	saveSettings();

	backupDb();

	delete accountModel;
	delete transferModel;

	delete ui;
}

void MainWindow::init() {
	openDb();
	Evolutions(db).run();
	tagHelper = new TagHelper(db, this);

	initMenu();

	setupAccountTab();
	setupTransferTab();
	connect(ui->tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

	// QTimer::singleShot(0, this, SLOT(importStatements()));

	// QTimer::singleShot(0, this, SLOT(onImportStatements()));
}

void MainWindow::initMenu() {
	connect(ui->actionAbout_Moneyphant, SIGNAL(triggered()), this, SLOT(onShowAbout()));
	connect(ui->actionExport_Transfers, SIGNAL(triggered()), this, SLOT(onExportTransfers()));
	connect(ui->actionImport_Statement_File, SIGNAL(triggered()), this, SLOT(onImportStatements()));
	connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(onShowPreferences()));
	connect(ui->actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
}

//
// Menu Actions
//
void MainWindow::onShowPreferences() {

}

void MainWindow::onImportStatements() {
	auto dir = settings.value("import/lastdir", QDir::home().absolutePath()).toString();
	auto filePath = QFileDialog::getOpenFileName(this, tr("Select one ore more statement files"), dir, "CSV-Files (*.csv);;Text Files (*.txt);;All Files (*)");
	if (filePath.isEmpty()) {
		return;
	}

	QFileInfo info(filePath);
	assert_error(info.exists(), "choosen file '%s' does not exists", cstr(filePath));
	settings.setValue("import/lastdir", info.absolutePath());

	StatementImporterDialog dialog(db, filePath, this);
	if (dialog.exec() == QDialog::Accepted) {
		auto f = dialog.format();

	}
}

void MainWindow::onExportTransfers() {

}

void MainWindow::onShowAbout() {
	AboutDialog dialog(this);
	dialog.exec();
}



void MainWindow::importStatements(QString folder) {
	if (folder.isEmpty()) {
		folder = settings.value("autoimport/folder", QDir::homePath()).toString();
	}
	QDir dir(folder);
	if (!dir.exists()) {
		assert_warning(false, "folder '%s' dosen't exists", cstr(folder));
		return;
	}

	StatementReader reader(db);
	reader.importMissingStatementFiles(folder);
}

void MainWindow::loadSettings() {
	settings.setFallbacksEnabled(false);

	QLocale::setDefault(QLocale(QLocale::German, QLocale::Germany));

	if (settings.value("mainwindow/fullscreen").toBool()) {
		showFullScreen();
	} else {
		setGeometry(settings.value("mainwindow/geometry").toRect());
	}
	int tabIndex = settings.value("mainwindow/tab").toInt();
	assert_error(tabIndex >= 0 && tabIndex < ui->tabs->count(), "invalid tab index %d in settings", tabIndex);
	ui->tabs->setCurrentIndex(tabIndex);
}

void MainWindow::saveSettings() {
	settings.setValue("mainwindow/geometry", geometry());
	settings.setValue("mainwindow/fullScreen", isFullScreen());
	settings.setValue("mainwindow/tab", ui->tabs->currentIndex());
}

void MainWindow::tabChanged(int index) {
	switch (index) {
		case 0:
			transferModel->reloadCache();
			break;
	}
}

void MainWindow::setCurrentAccount(const QModelIndex& idx) {
	assert_error(accountModel != nullptr);
	assert_error(accountProxyModel != nullptr);

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
	assert_error(accountModel != nullptr);
	assert_error(accountProxyModel != nullptr);
	assert_error(!selected.isEmpty() || !deseceted.isEmpty());

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
	assert_error(currentAccountId >= 0);

	const auto& ids = tagHelper->accountIds();
	auto selectionModel = ui->accountView->selectionModel();

	ui->accountTags->setEnabled(selectionModel->hasSelection());
	ui->moreAccountTags->setText("");
	ui->mergeAccounts->setVisible(ids.size() == 2);
	ui->actionMerge_Selected_Accounts->setEnabled(ids.size() == 2);

	if (ids.empty()) {
		return;
	} else if (ids.size() == 1) {
		ui->accountTags->setTags(tagHelper->getAccountTags(), tagHelper->accountIds());
	} else {
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

	updateAccountDetails();
}

void MainWindow::updateAccountDetails() {
	const auto& selectedIds = tagHelper->accountIds();
	bool editEnabled = selectedIds.size() == 1;

	ui->accountName->setEnabled(editEnabled);

	const auto& account = accountModel->getById(editEnabled ? selectedIds[0] : currentAccountId);

	ui->accountName->setText(account.name);
	ui->accountOwner->setText(account.owner);
	ui->accountIban->setText(account.iban);
	ui->accountBic->setText(account.bic);
	ui->accountAccountNumber->setText(account.accountNumber);
	ui->accountBankCode->setText(account.bankCode);
}

void MainWindow::mergeAccounts() {
	assert_error(tagHelper->accountIds().size() == 2);
	accountModel->mergeAccounts(tagHelper->accountIds().front(), tagHelper->accountIds().back());
	ui->accountView->selectionModel()->clearSelection();
	tagHelper->setAccountIds({});
}

void MainWindow::setCurrentTransfer(const QModelIndex& idx) {
	assert_error(transferModel != nullptr);
	assert_error(transferProxyModel != nullptr);

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
	assert_error(transferModel != nullptr);
	assert_error(transferProxyModel != nullptr);
	assert_error(!selected.isEmpty() || !deseceted.isEmpty());

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
	assert_error(currentTransferId >= 0);

	const auto& selectedIds = tagHelper->transferIds();
	const auto& ids = selectedIds.empty() ? std::vector<int>(1, currentTransferId) : selectedIds;

	ui->transferTags->setTags(tagHelper->getTransferTags(ids), ids);

	ui->checkSelectedTransfers->setVisible(!selectedIds.empty());

	updateTransferDetails();
}

void MainWindow::updateTransferDetails() {
	const auto& selectedIds = tagHelper->transferIds();
	bool editEnabled = selectedIds.size() == 1;

	const auto& transfer = transferModel->getById(editEnabled ? selectedIds[0] : currentTransferId);

	ui->transferFrom->setText(transfer.from.name);
	ui->transferReference->setText(transfer.reference);
	ui->transferNote->setPlainText(transfer.note);
	ui->tagsFromAccounts->setText(tagHelper->tagsFromAccounts(transfer.from.id, transfer.to.id).join("; "));
}

void MainWindow::resetTransferStats() {
	transferStats.clear();
	assert_error(transferStats.revenues() == 0 && transferStats.expenses() == 0 &&
					transferStats.internal() == 0 && transferStats.profit() == 0, "transfer statistics not cleared correctly");
	addToTransferStats(-1);
}

void MainWindow::addToTransferStats(int transferId) {
	if (transferId >= 0) {
		const auto& transfer = transferModel->getById(transferId);
		const auto& from = accountModel->getById(transfer.from.id);
		const auto& to = accountModel->getById(transfer.to.id);
		transferStats.add(transfer, transfer.internal || (from.isOwn && to.isOwn));
	}

	ui->trStatsRevenues->setText(currency(transferStats.revenues()));
	ui->trStatsExpenses->setText(currency(transferStats.expenses()));
	ui->trStatsInternal->setText(currency(transferStats.internal()));
	ui->trStatsProfit->setText(currency(transferStats.profit()));
}

void MainWindow::removeFromTransferStats(int transferId) {
	assert_error(transferId >= 0);

	const auto& transfer = transferModel->getById(transferId);
	const auto& from = accountModel->getById(transfer.from.id);
	const auto& to = accountModel->getById(transfer.to.id);
	transferStats.remove(transfer, transfer.internal || (from.isOwn && to.isOwn));

	ui->trStatsRevenues->setText(currency(transferStats.revenues()));
	ui->trStatsExpenses->setText(currency(transferStats.expenses()));
	ui->trStatsInternal->setText(currency(transferStats.internal()));
	ui->trStatsProfit->setText(currency(transferStats.profit()));
}

void MainWindow::saveTransferNote() {
	const auto& selectedIds = tagHelper->transferIds();
	const auto& id = selectedIds.size() == 1 ? selectedIds[0] : currentTransferId;
	auto note = ui->transferNote->toPlainText();
	transferModel->setNote(id, note);
}

void MainWindow::exportTransfers() {
	auto filename = QFileDialog::getSaveFileName(this, tr("Export Transfers"), QString(), tr("CSV (*.csv)"));
	if (filename.isEmpty()) return;
	bool hasSelection = ui->transferView->selectionModel()->hasSelection();
	auto ids = hasSelection ? tagHelper->transferIds() : transferStats.includedTransferIds();
	transferModel->exportTransfers(filename, ids);
}

void MainWindow::checkSelectedTransfers() {
	assert_debug(!tagHelper->transferIds().empty());
	transferModel->setChecked(tagHelper->transferIds(), true);
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

	// details edit
	connect(ui->transferNote, SIGNAL(textChanged()), this, SLOT(saveTransferNote()));

	// actions
	connect(ui->actionExport_Transfers, SIGNAL(triggered()), this, SLOT(exportTransfers()));
	connect(ui->checkSelectedTransfers, SIGNAL(clicked()), this, SLOT(checkSelectedTransfers()));
}

void MainWindow::clickedTransferFilterMonthLink() {
	auto senderButton = (QPushButton*)sender();
	auto text = senderButton->text();

	QDateTime start, end;
	if (text == "201x") {
		start = QDateTime(QDate(2010, 1, 1));
		end = QDateTime(QDate(2019, 12, 31), QTime(23, 59));
	} else if (text.length() == 4) {
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
		assert_error(!text.isEmpty());
		auto button = new QPushButton(text);
		connect(button, SIGNAL(pressed()), this, SLOT(clickedTransferFilterMonthLink()));
		ui->trFilterMonthLinks->addWidget(button);
	};

	addButton("201x");

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
	assert_error(dbConfig == nullptr);
	dbConfig = std::make_shared<sqlpp::sqlite3::connection_config>();
	assert_fatal(dbConfig != nullptr);

	QString dbPath = appLocalDataLocation() + "/db.sqlite";
	std::cout << dbPath << std::endl;
	dbConfig->path_to_database = str(dbPath);
	dbConfig->flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	dbConfig->debug = false;

	db = std::make_shared<sqlpp::sqlite3::connection>(dbConfig);
}

void MainWindow::backupDb() {
	QFile dbFile(appLocalDataLocation() + "/db.sqlite");
	QDir backupDir(appLocalDataLocation() + "/backups");
	if (!backupDir.exists()) {
		bool b = backupDir.mkpath(".");
		assert_error(b, "backup directory '%s' could not be created", cstr(backupDir.absolutePath()));
	}
	assert_error(backupDir.exists(), "backup directory '%s' doesn't exists and could not be created", cstr(backupDir.absolutePath()));
	assert_error(dbFile.exists(), "database file '%s' not found", cstr(dbFile.fileName()));

	auto now = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
	auto backupFileName = backupDir.absolutePath() + QDir::separator() + now + "_db.sqlite";
	bool success = dbFile.copy(backupFileName);
	assert_error(success, "could not copy database file '%s' to '%s'", cstr(dbFile.fileName()), cstr(backupFileName));
}

