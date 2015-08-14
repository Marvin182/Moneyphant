#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QStringList>
#include <QDateTime>
#include <QLabel>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QRegExp>
#include "Updater.h"
#include "ui/AboutDialog.h"
#include "ui/StatementImporterDialog.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	settings(mr::qt::appLocalDataLocation() + "/settings.ini", QSettings::IniFormat),
	dbConfig(nullptr),
	db(nullptr),
	tagHelper(nullptr, this),
	currentAccountId(-1),
	accountModel(nullptr),
	accountProxyModel(nullptr),
	statementReader(nullptr)
{
	ui->setupUi(this);
	tabs = {ui->transferTab, ui->accountTab};

	loadSettings();
	QTimer::singleShot(0, this, SLOT(init()));
}

MainWindow::~MainWindow() {
	saveSettings();
	backupDb();

	delete accountModel;
	delete ui;
}

void MainWindow::init() {
	openDb();
	Updater(db, settings).run();

	statementReader.setDb(db);
	tagHelper.setDb(db);

	initMenu();
	initAccountTab();
	ui->transferTab->init(db, accountModel);

	connect(ui->tabs, &QTabWidget::currentChanged, [&](int index) { if (index == 0) ui->transferTab->reloadCache(); });
	connect(&statementReader, &StatementReader::newStatementsImported, [&]() { accountModel->reloadCache(); ui->transferTab->reloadCache(); });

	QTimer::singleShot(0, &statementReader, SLOT(startWatchingFiles()));
}

void MainWindow::initMenu() {
	connect(ui->actionAbout_Moneyphant, SIGNAL(triggered()), this, SLOT(onShowAbout()));
	// connect(ui->actionExport_Transfers, SIGNAL(triggered()), this, SLOT(exportTransfers())); // TODO
	connect(ui->actionExport_Transfers, SIGNAL(triggered()), this, SLOT(onExportTransfers()));
	connect(ui->actionImport_Statement_File, SIGNAL(triggered()), this, SLOT(onImportStatements()));
	connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(onShowPreferences()));
	connect(ui->actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
	connect(ui->actionFind, &QAction::triggered, [&]() {
		switch (ui->tabs->currentIndex()) {
			case 0: return static_cast<TransferTab*>(tabs[0])->focuseSearchField();
			case 1: return ui->accFilterText->setFocus();
		}
	});
	connect(ui->actionTransfers, &QAction::triggered, [&]() { ui->tabs->setCurrentIndex(0); });
	connect(ui->actionAccounts, &QAction::triggered, [&]() { ui->tabs->setCurrentIndex(1); });
}

void MainWindow::initAccountTab() {
	// model and proxy model for filtering and sorting
	accountModel = new AccountModel(db, this);
	accountProxyModel = new AccountProxyModel(db, this);
	accountProxyModel->setSourceModel(accountModel);

	// filter edits
	connect(ui->accFilterText, SIGNAL(textChanged(const QString&)), accountProxyModel, SLOT(setFilterText(const QString&)));

	ui->accountView->setModel(accountProxyModel);
	ui->accountView->setSortingEnabled(true);

	// mouse tracking for hover
	ui->accountView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->accountView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	ui->accountView->setMouseTracking(true);
	connect(ui->accountView, SIGNAL(entered(const QModelIndex&)), this, SLOT(setCurrentAccount(const QModelIndex&)));
	connect(ui->accountView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(showSelectedAccounts(const QItemSelection&, const QItemSelection&)));
	connect(ui->accountTags, SIGNAL(tagsAdded(QStringList, const std::vector<int>&)), &tagHelper, SLOT(addAccountTags(QStringList, const std::vector<int>&)));
	connect(ui->accountTags, SIGNAL(tagsRemoved(QStringList, const std::vector<int>&)), &tagHelper, SLOT(removeAccountTags(QStringList, const std::vector<int>&)));
	connect(ui->actionMerge_Selected_Accounts, SIGNAL(triggered()), this, SLOT(mergeAccounts()));
	connect(ui->mergeAccounts, SIGNAL(clicked()), this, SLOT(mergeAccounts()));

	ui->mergeAccounts->setVisible(false);

	// configure headers
	ui->accountView->verticalHeader()->hide();
	ui->accountView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->accountView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	ui->accountView->horizontalHeader()->setDefaultSectionSize(60);
}

//
// Menu Actions
//
void MainWindow::onShowPreferences() {

}

void MainWindow::onImportStatements() {
	// ask user for the file to import
	auto dir = settings.value("import/lastdir", QDir::home().absolutePath()).toString();
	auto filePath = QFileDialog::getOpenFileName(this, tr("Select one ore more statement files"), dir, "CSV-Files (*.csv);;Text Files (*.txt);;All Files (*)");
	if (filePath.isEmpty()) {
		return;
	}

	// remember file path
	QFileInfo info(filePath);
	assert_error(info.exists(), "choosen file '%s' does not exists", cstr(filePath));
	settings.setValue("import/lastdir", info.absolutePath());

	// get file format
	StatementImporterDialog dialog(db, filePath, this);
	if (dialog.exec() != QDialog::Accepted) {
		return;
	}

	// do import
	statementReader.addFile(filePath, dialog.format(), dialog.watchFile());
}

void MainWindow::onExportTransfers() {

}

void MainWindow::onShowAbout() {
	AboutDialog dialog(this);
	dialog.exec();
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
		tagHelper.setAccountIds({currentAccountId});
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
		tagHelper.removeAccountId(id);
	}

	for (const auto& idx : selected.indexes()) {
		const auto& index = accountProxyModel->mapToSource(idx);
		if (index.column() != 0) {
			continue;
		}
		int id = accountModel->get(index.row()).id;
		tagHelper.addAccountId(id);
	}

	updateAccountInfo();
}
	
void MainWindow::updateAccountInfo() {
	assert_error(currentAccountId >= 0);

	const auto& ids = tagHelper.accountIds();
	auto selectionModel = ui->accountView->selectionModel();

	ui->accountTags->setEnabled(selectionModel->hasSelection());
	ui->moreAccountTags->setText("");
	ui->mergeAccounts->setVisible(ids.size() == 2);
	ui->actionMerge_Selected_Accounts->setEnabled(ids.size() == 2);

	if (ids.empty()) {
		return;
	} else if (ids.size() == 1) {
		ui->accountTags->setTags(tagHelper.getAccountTags(), tagHelper.accountIds());
	} else {
		const auto& commonTags = tagHelper.getAccountTags();
		ui->accountTags->setTags(commonTags, tagHelper.accountIds());

		const auto& currentAccountTags = tagHelper.getAccountTags({currentAccountId});
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
	const auto& selectedIds = tagHelper.accountIds();
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
	assert_error(tagHelper.accountIds().size() == 2);
	accountModel->mergeAccounts(tagHelper.accountIds().front(), tagHelper.accountIds().back());
	ui->accountView->selectionModel()->clearSelection();
	tagHelper.setAccountIds({});
}

void MainWindow::openDb() {
	assert_error(dbConfig == nullptr);
	dbConfig = std::make_shared<sqlpp::sqlite3::connection_config>();
	assert_fatal(dbConfig != nullptr);

	QString dbPath = mr::qt::appLocalDataLocation() + "/db.sqlite";
	qLog() << "Opening database " << dbPath;
	dbConfig->path_to_database = str(dbPath);
	dbConfig->flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	dbConfig->debug = false;

	db = std::make_shared<sqlpp::sqlite3::connection>(*dbConfig);
}

void MainWindow::backupDb() {
	QFile dbFile(mr::qt::appLocalDataLocation() + "/db.sqlite");
	QDir backupDir(mr::qt::appLocalDataLocation() + "/backups");
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

