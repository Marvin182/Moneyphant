#ifndef EVOLUTIONS_H
#define EVOLUTIONS_H

#include "globals/all.h"
#include <vector>
#include <QStringList>

struct Evolution {
	int id;
	QStringList ups;
	QStringList downs;
};

class Evolutions {
public:
	Evolutions(Db db);

	void run();

private:
	Db db;

	std::vector<Evolution> evolutions();

	void up(const Evolution& evolution);
	void down(const Evolution& evolution);
};

#endif // EVOLUTIONS_H
