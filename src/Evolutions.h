#ifndef EVOLUTIONS_H
#define EVOLUTIONS_H

#include <vector>
#include <QStringList>
#include <mr/common>
#include "sql.h"

struct Evolution {
	int id;
	QStringList ups;
	QStringList downs;

	Evolution();
	Evolution(int id, cqstring upCmds, cqstring downCmds);
};

class Evolutions {
public:
	Evolutions(Db db);

	void run();

private:
	Db db;

	std::pair<std::vector<Evolution>, std::vector<Evolution>> upAndDownsForDb(std::vector<Evolution> evolutions);
	std::vector<Evolution> evolutionsFromFiles();

	void executeUp(const Evolution& evolution);
	void executeDown(const Evolution& evolution);
};

#endif // EVOLUTIONS_H
