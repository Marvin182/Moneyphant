#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QSettings>
#include "../sql.h"
#include "../StatementReader.h"
#include "../model/AccountModel.h"
#include "../model/TransferModel.h"
#include "../ui-tabs/Tab.h"

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
	void onImportStatements();
	void onExportTransfers();
	void onShowAbout();
	void onShowPreferences();

private:
	Ui::MainWindow *ui;

	QSettings settings;

	Db db;
	std::shared_ptr<sqlpp::sqlite3::connection_config> dbConfig;

	std::shared_ptr<AccountModel> accountModel;
	std::shared_ptr<TransferModel> transferModel;
	
	StatementReader statementReader;

	void openDb();
	void backupDb();

	void loadSettings();
	void saveSettings();

	Tab* tab(int index) const;
};

#endif // MAINWINDOW_H
