#include "Evolutions.h"

#include <algorithm>
#include <QDir>
#include <QFile>
#include <QStringRef>
#include <QDateTime>
#include <QtDebug>

Evolution::Evolution() :
	id(-1),
	ups(),
	downs()
{}

Evolution::Evolution(int id, cqstring upCmds, cqstring downCmds) :
	id(id),
	ups(upCmds.split(";\n")),
	downs(downCmds.split(";\n"))
{
	for (auto& up : ups) {
		if (!up.endsWith(';')) up += ';';
	}
	for (auto& down : downs) {
		if (!down.endsWith(';')) down += ';';
	}

}

Evolutions::Evolutions(Db db) :
	db(db)
{}

void Evolutions::run() {
	// create evolution table, there is no up evolution for this
	db->execute("create table if not exists evolution (\
		id integer primary key,\
		upTs integer not null default 0,\
		downTs integer not null default 0,\
		ups text not null default '',\
		downs text not null default ''\
	);");

	auto upsAndDowns = upAndDownsForDb(evolutionsFromFiles());

	for (int i = upsAndDowns.second.size() - 1; i >= 0; i--) {
		executeDown(upsAndDowns.second[i]);
	}
	for (int i = 0; i < upsAndDowns.first.size(); i++) {
		executeUp(upsAndDowns.first[i]);
	}
}

std::vector<Evolution> Evolutions::evolutionsFromFiles() {
	Q_INIT_RESOURCE(evolutions);
	auto evoFiles = QDir(":/evolutions/").entryList(QStringList("*.sql"));
	assert_fatal(evoFiles.length() > 0, "no sql evolution files found");

	std::vector<Evolution> evos;
	for (const auto& filename : evoFiles) {
		Evolution evo;
		bool ok;
		evo.id = QStringRef(&filename, 0, filename.length() - 4).toString().toInt(&ok);
		assert_error(ok, "could not parse sql evolution id from '%s'", cstr(filename));
		assert_error(evo.id >= 0 && evo.id < 100000, "invalid sql evolution id");
		
		// open file
		QFile file(":/evolutions/" + filename);
		assert_debug(file.exists(), "sql evolution file '%s' does not exist", cstr(filename));
		bool b = file.open(QFile::ReadOnly | QFile::Text);
		assert_error(b, "could not open sql evolution file '%s'", cstr(filename));
		
		bool upPart = true;
		QString cmd("");
		cmd.reserve(1024);

		int lineNumber = 0;
		while (!file.atEnd()) {
			lineNumber++;
			auto line = file.readLine().trimmed();
			if (line.isEmpty()) {
				// ignore
			} else if (line.startsWith('#') || line.startsWith('-')) {
				// comment line
				if (line.contains("# --- !Ups")) {
					upPart = true;
				} else if (line.contains("# --- !Downs")) {
					upPart = false;
				}
			} else {
				cmd += line;
				if (line.endsWith(';')) {
					if (upPart) {
						evo.ups << cmd;
					} else {
						evo.downs << cmd;
					}
					cmd = "";
					cmd.reserve(1024);
				}
			}
		}

		assert_error(!evo.ups.isEmpty(), "no up definitions for sql evolution %d", evo.id);
		assert_error(!evo.downs.isEmpty(), "no down definitions for sql evolution %d", evo.id);

		evos.push_back(evo);
	}

	Q_CLEANUP_RESOURCE(evolutions);

	assert_fatal(evos.size() == evoFiles.length(), "there should be an sql evolution for every sql evolution file (evos: %d, files: %d", (int)evos.size(), evoFiles.length());
	return evos;
}

std::pair<std::vector<Evolution>, std::vector<Evolution>> Evolutions::upAndDownsForDb(std::vector<Evolution> evolutions) {
	std::sort(evolutions.begin(), evolutions.end(), [](const Evolution& evo1, const Evolution& evo2) { return evo1.id <= evo2.id; });
	assert_debug(evolutions.front().id == 0);
	assert_debug(evolutions.back().id == evolutions.size() - 1);

	std::vector<Evolution> upEvos, downEvos;
	db::Evolution evo;
	for (const auto& evolution : evolutions) {
		auto result = (*db)(select(all_of(evo)).from(evo).where(evo.id == evolution.id));
		if (result.empty()) {
			// Case 1: evolution missing, definitely needs a up
			(*db)(insert_into(evo).set(evo.id = evolution.id));
			upEvos.push_back(evolution);
		} else {
			// check if evolution has the same ups as expected
			auto ups = qstr(result.front().ups);
			auto upsExpected = evolution.ups.join('\n');
			if (ups == upsExpected) {
				if (result.front().upTs == 0) {
					upEvos.push_back(evolution);
					if (result.front().downTs != 0) {
						downEvos.push_back(Evolution(result.front().id, qstr(result.front().ups), qstr(result.front().downs)));
					}
				}
			} else {
				if (evolution.id == evolutions.back().id) {
					// newest evolution might change in dev/beta state, allow reapply
					downEvos.push_back(Evolution(result.front().id, qstr(result.front().ups), qstr(result.front().downs)));
					upEvos.push_back(evolution);
				} else {
					// evolutions are not meant to change, throw error
					assert_fatal(false, "Evolution %d changed, changes will not be applied as there are newer evolutions\n\tups in db: %s\n\tups in file: %s", evolution.id, cstr(ups), cstr(upsExpected));
				}
			}
		}
	}

	return std::make_pair(upEvos, downEvos);
}

void Evolutions::executeUp(const Evolution& evolution) {
	qInfo("Applying evolution %d", evolution.id);
	auto ups = str(evolution.ups.join('\n'));
	auto downs = str(evolution.downs.join('\n'));
	for (auto& up : evolution.ups) {
		assert_debug(up.endsWith(';'));
		try {
			qLog() << "db up execute: " << up;
			db->execute(str(up));
		} catch (sqlpp::exception e) {
			qCritical() << e.what();
			// throw e;
		}
	}

	long long now = QDateTime::currentMSecsSinceEpoch();
	db::Evolution evo;
	(*db)(update(evo).set(evo.upTs = now, evo.downTs = 0, evo.ups = ups, evo.downs = downs).where(evo.id == evolution.id));
}

void Evolutions::executeDown(const Evolution& evolution) {
	qInfo("Undoing evolution %d", evolution.id);
	auto ups = str(evolution.ups.join('\n'));
	auto downs = str(evolution.downs.join('\n'));
	for (auto& down : evolution.downs) {
		assert_debug(down.endsWith(';'), "down command ('%s') of evolution does not end with ';'", cstr(down));
		try {
			qLog() << "db down execute: " << down;
			db->execute(str(down));
		} catch (sqlpp::exception e) {
			if (down.contains("drop table") && QString(e.what()).contains("no such table")) {
				// table was already dropped, but maybe something else failed previously (e.g. syntax error in up command)
				// => ignore and continue
			} else {
				qCritical() << e.what();
				throw e;
			}
		}

	}

	long long now = QDateTime::currentMSecsSinceEpoch();
	db::Evolution evo;
	(*db)(update(evo).set(evo.upTs = 0, evo.downTs = now, evo.ups = ups, evo.downs = downs).where(evo.id == evolution.id));
}
