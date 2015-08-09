#ifndef STATEMENT_READER_H
#define STATEMENT_READER_H

#include <mr/common>
#include <QFile>
#include <QFileSystemWatcher>
#include "sql.h"
#include "model/StatementFileFormat.h"
#include "Account.h"
#include "Transfer.h"

class StatementReader : public QObject {
	Q_OBJECT
public:
	StatementReader(Db db);

	void setDb(Db db) { this->db = db; }

public slots:
	void startWatchingFiles();
	void importStatementFile(cqstring path, const StatementFileFormat& format);

protected:
	Db db;

	QStringList& addFieldsFromLineSuffix(QStringList& fields, const StatementFileFormat& format);

	Transfer::Acc acc(cqstring owner, cqstring iban, cqstring bic);

	int find(Account& account);
	int find(Transfer& transfer);
	int findOrAdd(Account& account);
	int findOrAdd(Transfer& transfer);

	int add(Account& account);
	int add(Transfer& transfer);
	void insert(const Account& account);
	void insert(const Transfer& account);

};

#endif // STATEMENT_READER_H
