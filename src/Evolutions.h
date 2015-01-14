#ifndef EVOLUTIONS_H
#define EVOLUTIONS_H

#include <vector>
#include <QStringList>
#include "Global.h"

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
