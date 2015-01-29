#ifndef UI_STATEMENT_IMPORTER_DIALOG_H
#define UI_STATEMENT_IMPORTER_DIALOG_H

#include "../globals/all.h"
#include <vector>
#include <QDialog>
#include <QFile>

namespace Ui {
	class StatementImporterDialog;
}
class DataChooser;

class StatementImporterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit StatementImporterDialog(Db db, cqstring filePath, QWidget *parent = 0);
	~StatementImporterDialog();

private slots:
	void onDataChooserChanged(int row, int index);
	void createDataChoosers();

private:
	Ui::StatementImporterDialog *ui;

	Db db;
	std::vector<QString> lines;
	std::vector<DataChooser*> dataChoosers;

	void readFile(cqstring filePath);
	int guessDelimiter();
	int guessTextQualifier();
};

#endif // UI_STATEMENT_IMPORTER_DIALOG_H
