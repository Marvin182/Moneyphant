#ifndef UPDATER_H
#define UPDATER_H

#include <QSettings>
#include "sql.h"

class Updater {
public:
	Updater(Db db, QSettings& settings);

	void run();

private:
	Db db;
	QSettings& settings;

	void beforeEvolutions(int build);
	void afterEvolatuons(int build);
	void dbMaintenance();
};

#endif // UPDATER_H
