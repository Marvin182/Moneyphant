#include "util.h"

#include <QDateTime>
#include <QRegExp>
#include <QtDebug>

namespace util {

std::string internalTransferHash(int accountId, int amount, qint64 dt) {
	auto date = QDateTime::fromMSecsSinceEpoch(dt, Qt::UTC).date();
	int year1 = date.addDays(-14).year();
	int month1 = date.addDays(-14).month();
	int year2 = date.addDays(+14).year();
	int month2 = date.addDays(+14).month();
	return QString(qnstr(accountId) % ";" % qnstr(amount) % ";" % qnstr(year1) % "." % qnstr(month1) % "-" % qnstr(year2) % "." % qnstr(month2)).toStdString();
};

int parseCurrency(cqstring s) {
	// 1) eliminate white space
	// 2) thousand separators (period or comma follow by at least 3 digits)
	// 3) replace comma with period
	// 4) test if negative and ignore minus for the rest 
	// 4) return 100 * [part before period] + [part after period]
	auto s_clean = QString(s).remove(QRegExp("\\s")).remove(QRegExp("[\\.,](?=\\d{3}.*)")).replace(',', '.');
	if (s_clean.isEmpty()) return 0;

	if (s_clean.count('.') > 1) {
		throw std::invalid_argument(QString("Could not parse '%1' ('%2') to currency").arg(s).arg(s_clean).toStdString());
	}
	int period_idx = s_clean.indexOf('.');
	int negative = s_clean[0] == '-' ? 1 : 0;

	bool ok1, ok2 = true;
	int before, after = 0;
	if (period_idx == -1) {
		// no decimal digits
		before = QStringRef(&s_clean, negative, s_clean.length() - negative).toInt(&ok1);
	} else if (period_idx == s_clean.length() - 1) {
		// no decimal digits, but last char is the decimal separator
		before = QStringRef(&s_clean, negative, s_clean.length() - negative - 1).toInt(&ok1);
	} else {
		// 1 or 2 decimal digits
		// period_idx < s_clean.length() and because of prior if clause also period_idx + 1 < s_clean.length()
		// period_idx + 1 is always a valid index
		before = QStringRef(&s_clean, negative, period_idx - negative).toInt(&ok1) ;
		after = QStringRef(&s_clean, period_idx + 1, s_clean.length() - period_idx - 1).toInt(&ok2);
		if (s_clean.length() - period_idx - 1 == 1) {
			// just one decimal digit
			after *= 10;
		}
		assert_error(after >= 0 && after <= 99, "s = '%s', s_clean = '%s', after = %d", cstr(s), cstr(s_clean), after);
	}
	assert_error(before >= 0, "s = '%s', s_clean = '%s', before = %d", cstr(s), cstr(s_clean), before);

	if (!ok1 || !ok2) {
		throw std::invalid_argument(QString("Could not parse '%1' ('%2', period_idx = %3) to currency").arg(s).arg(s_clean).arg(period_idx).toStdString());
	}

	if (negative) {
		return - (100 * before + after);
	}
	return 100 * before + after;
}

QColor nextBeautifulColor(const QColor& c) {
	auto h = c.hueF() + mr::constants::goldenRatio - 1;
	if (h > 1.0) h--;
	return QColor::fromHsv(360 * h, c.saturation(), c.value(), c.alpha());
}

QVector<QColor> randomColors(int n, int saturation, int value, int alpha) {
	double hue = mr::random::probability();
	QVector<QColor> colors;
	colors.reserve(n);
	for (int i = 0; i < n; i++) {
		colors += QColor(360 * hue, saturation, value, alpha);
		hue += mr::constants::goldenRatio - 1;
		if (hue > 1.0) hue--; 
	}
	return colors;
}
} // namespace util
