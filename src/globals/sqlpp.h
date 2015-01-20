#ifndef GLOBALS_DB_H
#define GLOBALS_DB_H

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>

using namespace sqlpp;
using Db = std::shared_ptr<sqlpp::sqlite3::connection>;

#endif // GLOBALS_DB_H

