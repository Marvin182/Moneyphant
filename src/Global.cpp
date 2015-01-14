#include "Global.h"

const char* cstr(string s) {
	return s.toLocal8Bit().constData();
}

std::string str(string s) {
	return s.toLocal8Bit().constData();
}

QString qstr(std::string s) {
	return s.c_str();
}

std::ostream& operator<<(std::ostream& os, const QString& s) {
	os << s.toUtf8().constData();
	return os;
}

qint64 nowTs() {
	return QDateTime::currentMSecsSinceEpoch();
}
