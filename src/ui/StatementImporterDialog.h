#ifndef UI_STATEMENT_IMPORTER_DIALOG_H
#define UI_STATEMENT_IMPORTER_DIALOG_H

#include "../globals/all.h"
#include <QDialog>
#include <QFile>

namespace Ui {
	class StatementImporterDialog;
}

class StatementImporterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit StatementImporterDialog(cqstring filePath, QWidget *parent = 0);
	~StatementImporterDialog();

private:
	Ui::StatementImporterDialog *ui;
	QFile file;
};

#endif // UI_STATEMENT_IMPORTER_DIALOG_H
