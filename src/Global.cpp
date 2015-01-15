#include "Global.h"

const char* cstr(string s) {
	return s.toUtf8().constData();
}

std::string str(string s) {
	return s.toStdString();
}

QString qstr(std::string s) {
	return QString::fromStdString(s);
}

std::ostream& operator<<(std::ostream& os, const QString& s) {
	os << s.toUtf8().constData();
	return os;
}

qint64 nowTs() {
	return QDateTime::currentMSecsSinceEpoch();
}
