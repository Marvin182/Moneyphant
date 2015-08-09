#ifndef UI_STATEMENT_IMPORTER_DIALOG_H
#define UI_STATEMENT_IMPORTER_DIALOG_H

#include <vector>
#include <QDialog>
#include <mr/common>
#include "../sql.h"
#include "../model/StatementFileFormat.h"
#include "ColumnChooser.h"

namespace Ui {
	class StatementImporterDialog;
}

class StatementImporterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit StatementImporterDialog(Db db, cqstring filePath, QWidget *parent = 0);
	~StatementImporterDialog();

	StatementFileFormat format() const { return _format; }
	bool watchFile() const;

private slots:
	void createColumnChoosers();
	void onColumnChooserChanged(int columnIndex, const QString& inputType);
	void onDelimiterChanged(int);
	void onTextQualifierChanged(int);
	void onSkipFirstLineChanged(int);
	void onLineSuffixChanged(const QString& newSuffix);

private:
	Ui::StatementImporterDialog *ui;

	bool _success;
	StatementFileFormat _format;

	Db db;
	std::vector<QString> lines;
	std::vector<ColumnChooser*> columnChoosers;

	bool hasTextQualifier() const;
	const QString& delimiter() const;
	const QString& textQualifier() const;
	int delimiterIndex(cqstring del) const;
	int textQualifierIndex(cqstring tq) const;

	std::vector<QString> readFile(cqstring filePath);
	QStringList lineAsStringList(cqstring line) const;

	QString guessDelimiter();
	QString guessTextQualifier();
};

#endif // UI_STATEMENT_IMPORTER_DIALOG_H
