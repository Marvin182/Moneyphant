#ifndef SQLPP_H
#define SQLPP_H

#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/sqlite3/sqlite3.h>

#include <sqlpp11/alias_provider.h>
#include <sqlpp11/column_types.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/update.h>
#include <sqlpp11/select.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/boolean_expression.h>

#include "db.h"

using namespace sqlpp;
using Db = std::shared_ptr<sqlpp::sqlite3::connection>;

#endif // SQLPP_H
