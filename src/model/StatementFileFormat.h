#ifndef UI_STATEMENT_FILE_FORMAT_H
#define UI_STATEMENT_FILE_FORMAT_H

#include <mr/common>
#include "../sql.h"
#include <QMap>

struct StatementFileFormat {
	int id;
	QString name;
	QString hashedHeader;
	QString delimiter;
	QString textQualifier;
	bool skipFirstLine;
	QString dateFormat;
	QMap<QString, int> columnPositions;
	QString lineSuffix;

	StatementFileFormat();

	void setHeader(cqstring header);

	bool isValid() const;

	bool load(Db db);
	int save(Db db);
};

std::ostream& operator<<(std::ostream& os, const StatementFileFormat& f);

#endif // UI_STATEMENT_FILE_FORMAT_H
