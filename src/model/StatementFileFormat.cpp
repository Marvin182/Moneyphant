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
	columnPositions()
{}

void StatementFileFormat::setHeader(cqstring header) {
	hashedHeader = QCryptographicHash::hash(header.toUtf8(), QCryptographicHash::Sha256);
}

bool StatementFileFormat::load(Db db) {
	db::Format fm;
	auto fms = db->run(select(all_of(fm)).from(fm).where(fm.hashedHeader == str(hashedHeader)));
	if (fms.empty()) {
		return false;
	}

	id = fms.front().id;
	name = qstr(fms.front().name);
	delimiter = qstr(fms.front().delimiter);
	textQualifier = qstr(fms.front().textQualifier);
	skipFirstLine = fms.front().skipFirstLine;
	
	for (auto s : qstr(fms.front().columnPositions).split(';', QString::SkipEmptyParts)) {
		assert_debug(!s.isEmpty());
		auto kv = s.split(':');
		assert_debug(kv.size() == 2);
		columnPositions[kv[0]] = kv[1].toInt();
	}

	return true;
}

int StatementFileFormat::save(Db db) {
	db::Format fm;

	QString cp;
	cp.reserve(20 * columnPositions.size());
	for (auto it = columnPositions.begin(); it != columnPositions.end(); it++) {
		cp += it.key() + ":" + QString::number(it.value()) + ";"; 
	}

	if (id == -1) {
		id = db->run(insert_into(fm).set(fm.name = str(name),
											fm.hashedHeader = str(hashedHeader),
											fm.delimiter = str(delimiter),
											fm.textQualifier = str(textQualifier),
											fm.skipFirstLine = skipFirstLine,
											fm.columnPositions = str(cp)));
		assert_error(id >= 0);
	} else {
		db->run(sqlpp::update(fm).set(fm.name = str(name),
									fm.hashedHeader = str(hashedHeader),
									fm.delimiter = str(delimiter),
									fm.textQualifier = str(textQualifier),
									fm.skipFirstLine = skipFirstLine,
									fm.columnPositions = str(cp)
									).where(fm.id == id));
	}

	return id;
}

std::ostream& operator<<(std::ostream& os, const StatementFileFormat& f) {
	os << "StatementFileFormat " << f.name
		<< " (#" << f.hashedHeader
		<< ", delimiter: " << f.delimiter
		<< ", text qualifier: " << f.textQualifier << ")";
	for (auto k : f.columnPositions.keys()) {
		os << "\n\t" << cstr(k) << " => " << f.columnPositions[k];
	}
	return os;
}
