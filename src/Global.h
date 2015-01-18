#ifndef GLOBAL_H
#define GLOBAL_H

#include <cassert>
#include <QString>
#include <QDateTime>

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>

using namespace sqlpp;
using Db = std::shared_ptr<sqlpp::sqlite3::connection>;

using string = const QString&;

const char* cstr(string s);
std::string str(string s);
QString qstr(std::string s);

QString euro(int amount);

std::ostream& operator<<(std::ostream& os, const QString& s);

qint64 nowTs();

#endif // GLOBAL_H
