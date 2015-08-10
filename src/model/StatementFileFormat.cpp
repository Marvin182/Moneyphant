#include "StatementFileFormat.h"

#include <QCryptographicHash>
#include <QStringList>

StatementFileFormat::StatementFileFormat() :
	id(-1),
	name(""),
	hashedHeader(""),
	delimiter(";"),
	textQualifier("\""),
	skipFirstLine(true),
	dateFormat("dd.MM.yy"),
	columnPositions(),
	lineSuffix("")
{}

bool StatementFileFormat::isValid() const {
	if (!columnPositions.contains("date")) {
		return false;
	}
	if (!columnPositions.contains("amount")) {
		return false;
	}
	if (!columnPositions.contains("receiverIban") && !columnPositions.contains("receiverId") && !columnPositions.contains("receiverEmail")) {
		return false;
	}

	return true;
}

void StatementFileFormat::setHeader(cqstring header) {
	hashedHeader = QCryptographicHash::hash(header.toUtf8(), QCryptographicHash::Sha256);
}

StatementFileFormat StatementFileFormat::loadFromDb(Db db, int id) {
	db::Format fm;
	auto fms = (*db)(select(all_of(fm)).from(fm).where(fm.id == id));
	assert_error(!fms.empty(), "could not load StatementFileFormat %d", id);

	StatementFileFormat f;
	f.id = fms.front().id;
	f.name = qstr(fms.front().name);
	f.delimiter = qstr(fms.front().delimiter);
	f.textQualifier = qstr(fms.front().textQualifier);
	f.skipFirstLine = fms.front().skipFirstLine;
	f.dateFormat = qstr(fms.front().dateFormat);
	mr::qt::deserialize(qstr(fms.front().columnPositions), f.columnPositions);
	f.lineSuffix = qstr(fms.front().lineSuffix);

	return f;
}


bool StatementFileFormat::load(Db db) {
	db::Format fm;
	auto fms = (*db)(select(all_of(fm)).from(fm).where(fm.hashedHeader == str(hashedHeader)));
	if (fms.empty()) {
		return false;
	}

	id = fms.front().id;
	name = qstr(fms.front().name);
	delimiter = qstr(fms.front().delimiter);
	textQualifier = qstr(fms.front().textQualifier);
	skipFirstLine = fms.front().skipFirstLine;
	dateFormat = qstr(fms.front().dateFormat);
	mr::qt::deserialize(qstr(fms.front().columnPositions), columnPositions);
	lineSuffix = qstr(fms.front().lineSuffix);

	return true;
}

int StatementFileFormat::save(Db db) {
	db::Format fm;

	if (id == -1) {
		id = (*db)(insert_into(fm).set(fm.name = str(name),
											fm.hashedHeader = str(hashedHeader),
											fm.delimiter = str(delimiter),
											fm.textQualifier = str(textQualifier),
											fm.skipFirstLine = skipFirstLine,
											fm.dateFormat = str(dateFormat),
											fm.columnPositions = str(mr::qt::serialize(columnPositions)),
											fm.lineSuffix = str(lineSuffix)
											));
		assert_error(id >= 0);
	} else {
		(*db)(sqlpp::update(fm).set(fm.name = str(name),
									fm.hashedHeader = str(hashedHeader),
									fm.delimiter = str(delimiter),
									fm.textQualifier = str(textQualifier),
									fm.skipFirstLine = skipFirstLine,
									fm.dateFormat = str(dateFormat),
									fm.columnPositions = str(mr::qt::serialize(columnPositions)),
									fm.lineSuffix = str(lineSuffix)
									).where(fm.id == id));
	}

	return id;
}

StatementFileFormat::operator QString() const {
	auto s = QString("StatementFileFormat %1 (delimiter: %3, text qualifier: %4, date format: %5)").arg(name).arg(delimiter).arg(textQualifier).arg(dateFormat);
	for (auto k : columnPositions.keys()) {
		s += QString("\n\t\t%1 => %2").arg(k).arg(columnPositions[k]);
	}
	return s;
}

std::ostream& operator<<(std::ostream& os, const StatementFileFormat& f) {
	os << "StatementFileFormat " << f.name
		<< "(delimiter: " << f.delimiter
		<< ", text qualifier: " << f.textQualifier
		<< ", date format: " << f.dateFormat << ")";
	for (auto k : f.columnPositions.keys()) {
		os << "\n\t" << cstr(k) << " => " << f.columnPositions[k];
	}
	return os;
}
