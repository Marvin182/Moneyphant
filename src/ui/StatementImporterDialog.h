#ifndef UI_STATEMENT_IMPORTER_DIALOG_H
#define UI_STATEMENT_IMPORTER_DIALOG_H

#include "mr/common.h"
#include "../sql.h"
#include "ColumnChooser.h"
#include <vector>
#include <QDialog>
#include <QFile>

namespace Ui {
	class StatementImporterDialog;
}

class StatementImporterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit StatementImporterDialog(Db db, cqstring filePath, QWidget *parent = 0);
	~StatementImporterDialog();

	const QString& delimiter() const;
	const QString& textQualifier() const;
	ColumnChooser::InputFormat columnsOrder() const;

	bool success() const { return _success; };

private slots:
	//bool validateFormat(db::Format);
	bool validateFormat();
	void doImport();
	void onColumnChooserChanged(int columnIndex, int inputTypeIndex);
	void createColumnChoosers();

private:
	Ui::StatementImporterDialog *ui;

	bool _success;

	Db db;
	std::vector<QString> lines;
	std::vector<ColumnChooser*> columnChoosers;

	void readFile(cqstring filePath);
	int guessDelimiter();
	int guessTextQualifier();
};

#endif // UI_STATEMENT_IMPORTER_DIALOG_H
