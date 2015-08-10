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

	bool isValid() const;
	void setHeader(cqstring header);

	static StatementFileFormat loadFromDb(Db db, int id);
	bool load(Db db);
	int save(Db db);

	operator QString() const;
};

std::ostream& operator<<(std::ostream& os, const StatementFileFormat& f);

#endif // UI_STATEMENT_FILE_FORMAT_H
