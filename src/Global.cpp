#include "Global.h"
#include <QLocale>

const char* cstr(string s) {
	return s.toUtf8().constData();
}

std::string str(string s) {
	return s.toStdString();
}

QString qstr(std::string s) {
	return QString::fromStdString(s);
}

QString euro(int amount) {
	return QLocale(QLocale::German, QLocale::Germany).toCurrencyString(amount / 100.0);
}

bool fuzzyContains(const QString& text, const QString& needle) {
	int j = 0;
	for (const auto& c : needle) {
		while (j < text.length() && c.toLower() != text[j].toLower()) {
			j++;
		}
		if (j >= text.length()) {
			return false;
		}
	}
	return true;
}


std::ostream& operator<<(std::ostream& os, const QString& s) {
	os << s.toUtf8().constData();
	return os;
}

qint64 nowTs() {
	return QDateTime::currentMSecsSinceEpoch();
}
