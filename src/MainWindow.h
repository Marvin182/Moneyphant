#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Global.h"
#include "TransferManager.h"
#include "AccountModel.h"
#include "TagHelper.h"
#include <QMainWindow>
#include <QSortFilterProxyModel>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void setCurrentAccount(const QModelIndex& index);
	void showSelectedAccounts(const QItemSelection& selected, const QItemSelection& deseceted);
	void updateAccountInfo();
	void updateAccountDetails(const Account& account);
	void mergeAccounts();

private:
	Ui::MainWindow *ui;

	std::shared_ptr<sqlpp::sqlite3::connection_config> dbConfig;
	Db db;

	int currentAccountId;
	TagHelper* tagHelper;
	AccountModel* accountModel;
	QSortFilterProxyModel* accountProxyModel;
	TransferManager* transferManager;

	void openDb();
	void setupAccountTab();
	void setupTransferTab();
};

#endif // MAINWINDOW_H
