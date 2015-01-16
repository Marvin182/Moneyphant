#ifndef STATEMENT_READER_H
#define STATEMENT_READER_H

#include "Global.h"
#include "Account.h"
#include "Transfer.h"
#include <QFile>

class StatementReader {
public:
	StatementReader(Db db);

	void importMissingStatementFiles(string folder);
	void importMissingStatementsCsv(string path);

protected:
	Db db;

	void parseMBSStatements(QFile& file);
	void parseAccountFile(QFile& file);
	void parseMastercardStatements(QFile& file);
	Transfer::Acc acc(string owner, string iban, string bic);

	int find(Account& account);
	int find(Transfer& transfer);

	int add(Account& account);
	void insert(Account& account);
	int add(Transfer& transfer);

	int findOrAdd(Account& account);
	int findOrAdd(Transfer& transfer);
};

#endif // STATEMENT_READER_H
