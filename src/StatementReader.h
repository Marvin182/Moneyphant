#ifndef STATEMENT_READER_H
#define STATEMENT_READER_H

#include "globals/all.h"
#include "globals/pempek_assert.h"
#include "Account.h"
#include "Transfer.h"
#include <QFile>

class StatementReader {
public:
	StatementReader(Db db);

	void importMissingStatementFiles(cqstring folder);
	void importMissingStatementsCsv(cqstring path);

protected:
	Db db;

	void parseMBSStatements(QFile& file);
	void parseAccountFile(QFile& file);
	void parseMastercardStatements(QFile& file);
	void parsePayPalStatements(QFile& file);

	template <typename F>
	void parseCsvFile(QFile& file, cqstring delimiter, F lineFunc) {
		auto separator = '"' + delimiter + '"';
		int lineNumber = 1;
		while (!file.atEnd()) {
			lineNumber++;
			auto line = QString(file.readLine());
			auto fields = line.split(separator);
			if (fields.isEmpty()) {
				continue;
			}
			auto& firstField = fields[0];
			auto& lastField = fields[fields.length() - 1];
			PPK_ASSERT_ERROR(firstField[0] == '"' && lastField[lastField.length() - 1] == '"', "bad formatted line '%s' in file %s line %d", cstr(line), cstr(file.fileName()), lineNumber);
			firstField = firstField.right(firstField.length() - 1);
			lastField = lastField.left(firstField.length() - 1);
			lineFunc(fields, lineNumber);
		}
	}

	Transfer::Acc acc(cqstring owner, cqstring iban, cqstring bic);

	int find(Account& account);
	int find(Transfer& transfer);

	int add(Account& account);
	void insert(Account& account);
	int add(Transfer& transfer);

	int findOrAdd(Account& account);
	int findOrAdd(Transfer& transfer);
};

#endif // STATEMENT_READER_H
