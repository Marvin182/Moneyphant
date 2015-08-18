#include "util.h"

#include <mr/common>
#include <QDateTime>

namespace util {

std::string internalTransferHash(int accountId, int amount, qint64 dt) {
	auto date = QDateTime::fromMSecsSinceEpoch(dt).date();
	int year1, year2;
	int week1 = date.addDays(-3).weekNumber(&year1);
	int week2 = date.addDays(+3).weekNumber(&year2);
	return QString(qnstr(accountId) % ";" % qnstr(amount) % ";" % qnstr(year1) % "." % qnstr(week1) % "-" % qnstr(year2) % "." % qnstr(week2)).toStdString();
};

} // namespace util