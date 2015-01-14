#include "Evolutions.h"

#include "Global.h"
#include "db.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStringRef>

namespace sql = sqlpp::sqlite3;

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

	std::vector<Evolution> evos;
	for (const auto& filename : evoFiles) {
		Evolution evo;
		evo.id = QStringRef(&filename, 0, filename.length() - 4).toString().toInt();
		
		// open file
		QFile file(":/evolutions/" + filename);
		assert(file.exists());
		bool b = file.open(QFile::ReadOnly | QFile::Text);
		assert(b);
		
		bool upPart = true;
		QString cmd("");
		cmd.reserve(1024);

		while (!file.atEnd()) {
			auto line = file.readLine().trimmed();
			if (line.isEmpty()) {
				// ignore
			} else if (line.startsWith("#")) {
				// comment line
				if (line.contains("# --- !Ups")) {
					upPart = true;
				} else if (line.contains("# --- !Downs")) {
					upPart = false;
				}
			} else {
				cmd += line;
				if (line.endsWith(";")) {
					assert(cmd[cmd.length() - 1] == ';' && cstr(cmd));
					// end of a command
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

		evos.push_back(evo);
	}

	Q_CLEANUP_RESOURCE(evolutions);

	return evos;
}

void Evolutions::up(const Evolution& evolution) {
	db::Evolution evo;
	std::string ups = evolution.ups.join('\n').toLocal8Bit().constData();
	std::string downs = evolution.downs.join('\n').toLocal8Bit().constData();
	// printf("ups = %s\n", ups.c_str());
	// printf("downs = %s\n", downs.c_str());
	for (auto& up : evolution.ups) {
		assert(up[up.length() - 1] == ';');
		db->execute(str(up));
	}
	db->run(insert_into(evo).set(evo.id = evolution.id, evo.upTs = (long long) nowTs, evo.ups = ups, evo.downs = downs));
}

void Evolutions::down(const Evolution& evolution) {
	db::Evolution evo;
	auto ups = evolution.ups.join('\n');
	auto downs = evolution.downs.join('\n');
	for (auto& down : evolution.downs) {
		db->execute(str(down));
	}
	db->run(insert_into(evo).set(evo.id = evolution.id, evo.upTs = 0, evo.downTs = (long long) nowTs, evo.ups = str(ups), evo.downs = str(downs)));
}
