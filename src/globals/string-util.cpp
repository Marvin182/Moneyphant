#include "string-util.h"
#include "assert.h"
#include <QLocale>

std::ostream& operator<<(std::ostream& os, cqstring s) {
	return (os << s.toStdString());
}

const char* cstr(cqstring s) {
	return s.toUtf8().constData();
}

std::string str(cqstring s) {
	return s.toStdString();
}

QString qstr(const std::string& s) {
	return QString::fromStdString(s);
}

QString currency(int amount, cqstring symbol) {
	return QLocale().toCurrencyString(amount / 100.0, symbol);
}

bool fuzzyMatch(cqstring text, cqstring needle) {
	int j = 0;
	for (const auto& chr : needle) {
		const auto c = chr.toLower();
		while (j < text.length() && text[j].toLower() != c) {
			j++;
		}
		if (j >= text.length()) {
			return false;
		}
	}
	return true;
}


