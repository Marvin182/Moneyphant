#include "StatementReader.h"
#include <QDir>
#include <QtDebug>
#include <QStringList>
#include "sql.h"
#include "TagHelper.h"


StatementReader::StatementReader(Db db) :
	db(db)
{
	connect(&fileWatcher, SIGNAL(fileChanged(const QString&)), SLOT(importStatementFile(const QString&)));
}

void StatementReader::startWatchingFiles() {
	db::File f;
	for (const auto& file : (*db)(select(all_of(f)).from(f).where(f.watch))) {
		importStatementFile(qstr(file.path), StatementFileFormat::loadFromDb(db, file.formatId), false);
		fileWatcher.addPath(qstr(file.path));
	}

	emit newStatementsImported();
}

void StatementReader::stopWatchingFiles() {
	fileWatcher.removePaths(fileWatcher.files());
}

void StatementReader::addFile(cqstring filename, const StatementFileFormat& format, bool watch) {
	db::File f;

	// remember file for auto import on startup (and watching)
	auto fs = (*db)(select(f.id, f.watch).from(f).where(f.path == str(filename)));
	if (fs.empty()) {
		(*db)(insert_into(f).set(f.path = str(filename),
									f.formatId = format.id,
									f.watch = (int)watch));
		if (watch) {
			// new file that should be watched
			qLog() << "watching file " << filename;
			fileWatcher.addPath(filename);
		}
	} else {
		(*db)(sqlpp::update(f).set(f.formatId = format.id,
								f.watch = (int)watch
								).where(f.id == fs.front().id));
		if (watch && !fs.front().watch) {
			// file should be watched, but isn't so far
			qLog() << "watching file " << filename;
			fileWatcher.addPath(filename);
		} else if (!watch && fs.front().watch) {
			// file shouldn't be watched, but is currently
			fileWatcher.removePath(filename);
		}
	}

	importStatementFile(filename, format);
}

void StatementReader::importStatementFile(cqstring filename) {
	qLog() << "change in " << filename << " detected";
	db::File f;
	auto files = (*db)(select(all_of(f)).from(f).where(f.path == str(filename)));
	assert_error(!files.empty(), "Could not auto import watched file %s (reason: file not found in db)", cstr(filename));
	importStatementFile(qstr(files.front().path), StatementFileFormat::loadFromDb(db, files.front().formatId));
}

void StatementReader::importStatementFile(cqstring filename, const StatementFileFormat& format, bool emitSignal) {
	qLog() << "importing " << filename << " with format " << format;

	mr::io::parseCsvFile(filename, format.delimiter, format.textQualifier, format.skipFirstLine, [&](int lineNumber, QStringList& fields) {
		// add suffix with additional fields (e.g. for default values)
		fields = addFieldsFromLineSuffix(fields, format);

		// define a shortcut for easy access to specific fields
		int defaultPos = fields.size();
		fields.append("");
		auto hasVal = [&](cqstring key) { return format.columnPositions.contains(key); };
		auto val = [&](cqstring key) { return fields[format.columnPositions.value(key, defaultPos)]; };

		// valid keys (also see ui/ColumnsChooser.cpp):
		// "id", "date", "amount", "reference", "senderOwnerName", "senderIban", "senderBic", "senderId", "receiverOwnerName", "receiverIban", "receiverBic", "receiverId", "note", "checked", "internal"

		auto date = QDateTime::fromString(val("date"), format.dateFormat);
		auto from = hasVal("senderId") ? Transfer::Acc(val("senderId").toInt(), "") : acc(val("senderOwnerName"), val("senderIban"), val("senderBic"));
		auto to = hasVal("receiverId") ? Transfer::Acc(val("receiverId").toInt(), "") : acc(val("receiverOwnerName"), val("receiverIban"), val("receiverBic"));
		int amount = val("amount").replace(',', '.').toDouble() * 100;
		
		assert_error(from.id >= 0 && to.id >= 0, "from %d, to: %d", from.id, to.id);
		assert_debug(from.id != to.id, "accounts are not allowed to match (lineNumber: %d, from: %s, to: %s)", lineNumber, cstr(from), cstr(to));

		Transfer t;
		if (hasVal("id")) {
			t = Transfer(val("id").toInt(), date, from, to, val("reference"), amount, val("reference"), val("checked") == "1", val("internal") == "1");
			insert(t);
		} else {
			t = Transfer(date, from, to, val("reference"), amount);
			findOrAdd(t);
		}
		assert_error(t.id >= 0);

		if (hasVal("tags")) {
			TagHelper th(db);
			th.addTransferTags(TagHelper::splitTags(val("tags")), {t.id});
		}
	});

	if (emitSignal) { 
		emit newStatementsImported();
	}
}

QStringList& StatementReader::addFieldsFromLineSuffix(QStringList& fields, const StatementFileFormat& format) {
	// normally the suffix will start a new column, which would cause the split() function to output an empty string as first part, which we don't want to have
	auto suffix = format.lineSuffix;
	if (suffix.startsWith(format.delimiter)) {
		suffix = suffix.right(suffix.length() - format.delimiter.length());
	}

	// special cases
	if (suffix.isEmpty()) return fields;
	if (format.textQualifier.isEmpty()) return fields << suffix.split(format.delimiter);

	return fields << mr::split(suffix, format.delimiter, format.textQualifier);
}

Transfer::Acc StatementReader::acc(cqstring owner, cqstring iban, cqstring bic) {
	Account a(owner, iban, bic);
	findOrAdd(a);
	assert_error(a.id >= 0);
	return Transfer::Acc(a.id, a.name);
}


int StatementReader::find(Account& account) {
	db::Account acc;
	auto accsSql = dynamic_select(*(db.get()), acc.id).from(acc).dynamic_where();

	assert_error(!account.iban.isEmpty() || !account.bic.isEmpty() || !account.accountNumber.isEmpty() || !account.bankCode.isEmpty());

	if (!account.iban.isEmpty() || account.accountNumber.isEmpty()) {
		accsSql.where.add(acc.iban == str(account.iban));
	}
	if (!account.bic.isEmpty()) {
		accsSql.where.add(acc.bic == str(account.bic));
	}
	if (!account.accountNumber.isEmpty()) {
		accsSql.where.add(acc.accountNumber == str(account.accountNumber));
	}
	if (!account.bankCode.isEmpty()) {
		accsSql.where.add(acc.bankCode == str(account.bankCode));
	}

	auto accs = (*db)(accsSql);

	if (!accs.empty()) {
		int id = accs.front().id;
		assert_error(id >= 0);
		account.id = id;
		return id;
	}

	return -1;
}

int StatementReader::find(Transfer& transfer) {
	db::Transfer tr;
	auto trs = (*db)(select(tr.id).from(tr).where(tr.date == transfer.dateMs() and
											tr.fromId == transfer.from.id and
											tr.toId == transfer.to.id and
											tr.reference == str(transfer.reference) and
											tr.amount == transfer.amount));
	if (!trs.empty()) {
		int id = trs.front().id;
		assert_error(id >= 0);
		transfer.id = id;
		return id;
	}
	return -1;
}

int StatementReader::findOrAdd(Account& account) {
	int id = find(account);
	if (id == -1) {
		id = add(account);
	}
	assert_error(id >= 0);
	return id;
}

int StatementReader::findOrAdd(Transfer& transfer) {
	int id = find(transfer);
	if (id == -1) {
		id = add(transfer);
	}
	assert_error(id >= 0);
	return id;
}

int StatementReader::add(Account& account) {
	assert_error(account.id == -1, "cannot add account that has already an id (id: %d, name: %s)", account.id, cstr(account.name));

	db::Account acc;
	int id = (*db)(insert_into(acc).set(acc.isOwn = account.isOwn,
										acc.name = str(account.name),
										acc.owner = str(account.owner),
										acc.iban = str(account.iban),
										acc.bic = str(account.bic),
										acc.accountNumber = str(account.accountNumber),
										acc.bankCode = str(account.bankCode)));
	assert_error(id >= 0);
	account.id = id;
	qLog() << "added " << account;
	return id;
}

int StatementReader::add(Transfer& transfer) {
	db::Transfer tr;
	int id = (*db)(insert_into(tr).set(tr.date = transfer.dateMs(),
										tr.fromId = transfer.from.id,
										tr.toId = transfer.to.id,
										tr.reference = str(transfer.reference),
										tr.amount = transfer.amount));
	assert_error(id >= 0);
	transfer.id = id;
	qLog() << "added " << transfer;
	return id;
}

void StatementReader::insert(const Account& account) {
	db::Account acc;
	assert_error(account.id >= 0);
	assert_error((*db)(select(count(acc.id)).from(acc).where(acc.id == account.id)).front().count == 0, "there exists already an account with id %d", account.id);
	
	(*db)(insert_into(acc).set(acc.id = account.id,
								acc.isOwn = account.isOwn,
								acc.name = str(account.name),
								acc.owner = str(account.owner),
								acc.iban = str(account.iban),
								acc.bic = str(account.bic),
								acc.accountNumber = str(account.accountNumber),
								acc.bankCode = str(account.bankCode)
								));
	qLog() << "inserted " << account;
}

void StatementReader::insert(const Transfer& transfer) {
	db::Transfer tr;
	assert_error(transfer.id >= 0);
	assert_error((*db)(select(count(tr.id)).from(tr).where(tr.id == transfer.id)).front().count == 0, "there exists already a transfer with id %d", transfer.id);

	(*db)(insert_into(tr).set(tr.id = transfer.id,
										tr.date = transfer.dateMs(),
										tr.fromId = transfer.from.id,
										tr.toId = transfer.to.id,
										tr.reference = str(transfer.reference),
										tr.amount = transfer.amount,
										tr.note = str(transfer.note),
										tr.checked = transfer.checked
										));

	qLog() << "inserted " << transfer;
}
