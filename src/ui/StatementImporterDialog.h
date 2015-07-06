#ifndef UI_STATEMENT_IMPORTER_DIALOG_H
#define UI_STATEMENT_IMPORTER_DIALOG_H

#include <vector>
#include <QDialog>
#include "mr/common.h"
#include "../sql.h"
#include "ColumnChooser.h"

namespace Ui {
	class StatementImporterDialog;
}

struct StatementFileFormat {
	int id;
	QString name;
	QString hashedHeader;
	QString delimiter;
	QString textQualifier;
	bool skipFirstLine;
	QMap<QString, int> columnPositions;
};

class StatementImporterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit StatementImporterDialog(Db db, cqstring filePath, QWidget *parent = 0);
	~StatementImporterDialog();

	StatementFileFormat format() const { return _format; }

private slots:
	void createColumnChoosers();
	void onColumnChooserChanged(int columnIndex, const QString& inputType);
	void onDelimiterChanged(int);
	void onTextQualifierChanged(int);
	void onSkipFirstLineChanged(int state);
	void saveFormatToDb();
	bool validateFormat(const StatementFileFormat& f);

private:
	Ui::StatementImporterDialog *ui;

	bool _success;
	StatementFileFormat _format;

	Db db;
	std::vector<QString> lines;
	std::vector<ColumnChooser*> columnChoosers;

	bool loadFromDb(StatementFileFormat& f);
	void saveFormatToDb(StatementFileFormat& f);

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

std::ostream& operator<<(std::ostream& os, const StatementFileFormat& f);

#endif // UI_STATEMENT_IMPORTER_DIALOG_H
