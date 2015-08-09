#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <mr/common>
#include "sql.h"
#include "AccountModel.h"
#include "AccountProxyModel.h"
#include "TransferModel.h"
#include "TransferProxyModel.h"
#include "TransferStats.h"
#include "TagHelper.h"
#include "StatementReader.h"
#include <QMainWindow>
#include <QSettings>
#include <QSortFilterProxyModel>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void init();
	void initMenu();

public slots:
	// menu actions
	void onShowPreferences();
	void onImportStatements();
	void onExportTransfers();
	void onShowAbout();

	void importStatements(QString folder = QString());

	void tabChanged(int index);

	void setCurrentAccount(const QModelIndex& index);
	void showSelectedAccounts(const QItemSelection& selected, const QItemSelection& deseceted);
	void updateAccountInfo();
	void updateAccountDetails();
	void mergeAccounts();
	
	void setCurrentTransfer(const QModelIndex& index);
	void showSelectedTransfers(const QItemSelection& selected, const QItemSelection& deseceted);
	void updateTransferTags();
	void updateTransferDetails();
	void saveTransferNote();
	void exportTransfers();
	void checkSelectedTransfers();

	void resetTransferStats();
	void addToTransferStats(int transferId);
	void removeFromTransferStats(int transferId);
	void clickedTransferFilterMonthLink();

	void backupDb();

private:
	Ui::MainWindow *ui;
	QSettings settings;

	std::shared_ptr<sqlpp::sqlite3::connection_config> dbConfig;
	Db db;

	TagHelper* tagHelper;
	
	int currentAccountId;
	AccountModel* accountModel;
	AccountProxyModel* accountProxyModel;
	
	int currentTransferId;
	TransferModel* transferModel;
	TransferProxyModel* transferProxyModel;

	TransferStats transferStats;
	StatementReader statementReader;

	void openDb();
	void loadSettings();
	void saveSettings();
	void setupAccountTab();
	void setupTransferTab();
	void createTransferFilterMonthLinks();
};

#endif // MAINWINDOW_H
