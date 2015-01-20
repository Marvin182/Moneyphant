#include "Evolutions.h"

#include "db.h"
#include <QDir>
#include <QFile>
#include <QStringRef>
#include <QDateTime>

Evolutions::Evolutions(Db db) :
	db(db)
{}

void Evolutions::run() {
	// create evolution table, there is no up evolution for this
	db->execute("create table if not exists evolution (\
		id integer primary key,\
		upTs integer not null default 0,\
		downTs integer not null default 0,\
		ups text not null,\
		downs text not null\
	);");

	db::Evolution evo;
	for (const auto& evolution : evolutions()) {
		auto result = db->run(select(all_of(evo)).from(evo).where(evo.id == evolution.id));
		if (result.empty()) {
			up(evolution);
		}
	}
}

std::vector<Evolution> Evolutions::evolutions() {
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

void Evolutions::up(const Evolution& evolution) {
	auto ups = str(evolution.ups.join('\n'));
	auto downs = str(evolution.downs.join('\n'));
	for (auto& up : evolution.ups) {
		assert_debug(up.endsWith(';'));
		db->execute(str(up));
	}

	long long now = QDateTime::currentMSecsSinceEpoch();
	db::Evolution evo;
	db->run(insert_into(evo).set(evo.id = evolution.id, evo.upTs = now, evo.ups = ups, evo.downs = downs));
}

void Evolutions::down(const Evolution& evolution) {
	auto ups = evolution.ups.join('\n');
	auto downs = evolution.downs.join('\n');
	for (auto& down : evolution.downs) {
		assert_debug(down.endsWith(';'));
		db->execute(str(down));
	}

	long long now = QDateTime::currentMSecsSinceEpoch();
	db::Evolution evo;
	db->run(insert_into(evo).set(evo.id = evolution.id, evo.upTs = 0, evo.downTs = now, evo.ups = str(ups), evo.downs = str(downs)));
}
