#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QFileDialog>
#include <mr/common>
#include "../Updater.h"
#include "../ui-dialogs/AboutDialog.h"
#include "../ui-dialogs/StatementImporterDialog.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	settings(mr::qt::appLocalDataLocation() + "/settings.ini", QSettings::IniFormat),
	db(nullptr),
	dbConfig(nullptr),
	accountModel(nullptr),
	transferModel(nullptr),
	statementReader(nullptr)
{
	ui->setupUi(this);

	loadSettings();
	QTimer::singleShot(0, this, SLOT(init()));
}

MainWindow::~MainWindow() {
	saveSettings();
	backupDb();

	delete ui;
}

void MainWindow::init() {
	openDb();
	Updater(db, settings).run();

	// initialize models
	accountModel = std::make_shared<AccountModel>(db);
	transferModel = std::make_shared<TransferModel>(db);
	connect(&statementReader, &StatementReader::newStatementsImported, [&]() { 
		accountModel->invalidateCache();
		transferModel->invalidateCache();
	});
	connect(accountModel.get(), &AccountModel::accountsMerged, transferModel.get(), &TransferModel::onAccountsMerged);

	// initialize tabs
	ui->transferTab->init(db, transferModel);
	ui->accountTab->init(db, accountModel);
	ui->balanceTab->init(db);
	connect(ui->tabs, &QTabWidget::currentChanged, [&](int index) { tab(index)->refresh(); });

	initMenu();

	// initialize statement reader and activate auto import
	statementReader.setDb(db);
	QTimer::singleShot(0, &statementReader, SLOT(startWatchingFiles()));
}

void MainWindow::initMenu() {
	connect(ui->actionAbout_Moneyphant, SIGNAL(triggered()), SLOT(onShowAbout()));
	connect(ui->actionExport_Transfers, SIGNAL(triggered()), SLOT(onExportTransfers()));
	connect(ui->actionImport_Statement_File, SIGNAL(triggered()), SLOT(onImportStatements()));
	connect(ui->actionPreferences, SIGNAL(triggered()), SLOT(onShowPreferences()));
	connect(ui->actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
	connect(ui->actionFind, &QAction::triggered, [&]() { tab(ui->tabs->currentIndex())->focusSearchField(); });
	connect(ui->actionTransfers, &QAction::triggered, [&]() { ui->tabs->setCurrentIndex(0); });
	connect(ui->actionAccounts, &QAction::triggered, [&]() { ui->tabs->setCurrentIndex(1); });
	connect(ui->actionBalance, &QAction::triggered, [&]() { ui->tabs->setCurrentIndex(2); });
	connect(ui->accountTab, &AccountTab::mergeAccountsEnabled, [&](bool enabled) { ui->actionMerge_Selected_Accounts->setEnabled(enabled); });
	connect(ui->actionMerge_Selected_Accounts, &QAction::triggered, ui->accountTab, &AccountTab::mergeAccounts);

	connect(ui->actionCheck_Transfers, &QAction::triggered, [&]() { ui->transferTab->toggleCheckedOfSelected(); });
	connect(ui->actionMark_Transfers_Internal, &QAction::triggered, [&]() { ui->transferTab->toggleInternalOfSelected(); });
	connect(ui->actionDelete_Transfers, &QAction::triggered, [&]() { ui->transferTab->removeSelected(); });
}

void MainWindow::onExportTransfers() {
	// TODO
	assert_warning("not yet implemented");
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

void MainWindow::onShowAbout() {
	AboutDialog dialog(this);
	dialog.exec();
}

void MainWindow::onShowPreferences() {

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

void MainWindow::loadSettings() {
	settings.setFallbacksEnabled(false);

	QLocale::setDefault(QLocale(settings.value("global/language", "de").toString()));

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

Tab* MainWindow::tab(int index) const {
	return (Tab*)ui->tabs->widget(index);
}
