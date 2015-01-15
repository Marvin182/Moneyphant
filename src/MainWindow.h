#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Global.h"
#include "TransferManager.h"
#include "AccountModel.h"
#include "Tag.h"
#include <QMainWindow>
#include <QModelIndex>

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
	void onAccountEntered(const QModelIndex& index);
	void addAccountTag();
	void removeAccountTag(Tag t);
	
private:
	Ui::MainWindow *ui;

	std::shared_ptr<sqlpp::sqlite3::connection_config> dbConfig;
	Db db;

	int currentAccountId;
	AccountModel* accountModel;
	TransferManager* transferManager;

	void openDb();
	void setupAccountTab();
	void setupTransferTab();
};

#endif // MAINWINDOW_H
