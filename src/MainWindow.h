#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <mr/common>
#include <QMainWindow>
#include <QSettings>
#include <QSortFilterProxyModel>

#include "sql.h"
#include "AccountModel.h"
#include "AccountProxyModel.h"
#include "TagHelper.h"
#include "StatementReader.h"

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
	void initAccountTab();

public slots:
	// menu actions
	void onShowPreferences();
	void onImportStatements();
	void onExportTransfers();
	void onShowAbout();

	void setCurrentAccount(const QModelIndex& index);
	void showSelectedAccounts(const QItemSelection& selected, const QItemSelection& deseceted);
	void updateAccountInfo();
	void updateAccountDetails();
	void mergeAccounts();
	// void exportTransfers(); // TODO
	
	void backupDb();

private:
	Ui::MainWindow *ui;
	QSettings settings;

	std::shared_ptr<sqlpp::sqlite3::connection_config> dbConfig;
	Db db;

	TagHelper tagHelper;
	
	int currentAccountId;
	AccountModel* accountModel;
	AccountProxyModel* accountProxyModel;
	
	StatementReader statementReader;

	void openDb();
	void loadSettings();
	void saveSettings();
};

#endif // MAINWINDOW_H
