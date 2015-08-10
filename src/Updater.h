#ifndef UPDATER_H
#define UPDATER_H

#include <mr/common>
#include <QSettings>
#include "sql.h"

class Updater {
public:
	Updater(Db db, QSettings& settings);

	void run();

private:
	Db db;
	QSettings& settings;

	void beforeEvolutions(int version);
	void afterEvolatuons(int version);
};

#endif // UPDATER_H
