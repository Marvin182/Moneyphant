#ifndef UI_STATEMENT_FILE_FORMAT_H
#define UI_STATEMENT_FILE_FORMAT_H

#include <QMap>
#include <mr/common>
#include <mr/qt>
#include "../sql.h"

struct StatementFileFormat {
	int id;
	QString name;
	QString hashedHeader;
	QString delimiter;
	QString textQualifier;
	bool skipFirstLine;
	QString dateFormat;
	QMap<QString, int> columnPositions; // actually we should use a QHash which is more like std::unordered_map, but we less than 20 elements QMap is actually faster
	QString lineSuffix;
	bool invertAmount;

	StatementFileFormat();

	bool isValid() const;
	void setHeader(cqstring header);

	static StatementFileFormat loadFromDb(Db db, int id);
	bool load(Db db);
	int save(Db db);

	operator QString() const;

protected:
	template <typename T>
	static void copyFromSql(const T& obj, StatementFileFormat& f) {
		f.id = obj.id;
		f.name = qstr(obj.name);
		f.delimiter = qstr(obj.delimiter);
		f.textQualifier = qstr(obj.textQualifier);
		f.skipFirstLine = obj.skipFirstLine;
		f.dateFormat = qstr(obj.dateFormat);
		mr::qt::deserialize(qstr(obj.columnPositions), f.columnPositions);
		f.lineSuffix = qstr(obj.lineSuffix);
		f.invertAmount = obj.invertAmount;
	}
};

std::ostream& operator<<(std::ostream& os, const StatementFileFormat& f);

#endif // UI_STATEMENT_FILE_FORMAT_H
