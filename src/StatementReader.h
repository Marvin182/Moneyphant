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
	StatementReader(Db db = nullptr);

	void setDb(Db db) { this->db = db; }

public slots:
	void startWatchingFiles();
	void stopWatchingFiles();
	void stopWatchingFile(cqstring filename);
	void addFile(cqstring filename, const StatementFileFormat& format, bool watch);
	void importStatementFile(cqstring path);

signals:
	void newStatementsImported();

protected:
	Db db;
	QFileSystemWatcher fileWatcher;

	void importStatementFile(cqstring path, const StatementFileFormat& format, bool emitSignal = true);
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
