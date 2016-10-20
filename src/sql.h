#ifndef SQL_H
#define SQL_H

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
#include <sqlpp11/schema_qualified_table.h>

#include "db.h"

using Db = std::shared_ptr<sqlpp::sqlite3::connection>;

/*
Useful commands for sqlite
--------------------------
Export dump for db: echo '.dump' | sqlite3 db.sqlite > test.dump
Import dump into new db: cat test.dump | sqlite3 db2.sqlite
Update database schema structure: sql/ddl2cpp sql/structure.sq src/db db
*/

#endif // SQL_H

