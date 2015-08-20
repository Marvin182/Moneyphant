#include "StatementReader.h"

#include <unordered_set>
#include <QSet>
#include <QDir>
#include <QtDebug>
#include <QStringList>
#include "sql.h"
#include "util.h"
#include "TagHelper.h"
#include "model/AccountModel.h"

StatementReader::StatementReader(Db db) :
	db(db),
	fileWatcher(0)
{
	connect(&fileWatcher, SIGNAL(fileChanged(const QString&)), SLOT(importStatementFile(const QString&)));
}

void StatementReader::startWatchingFiles() {
	db::File f;
	for (const auto& file : (*db)(select(all_of(f)).from(f).where(f.watch))) {
		importStatementFile(qstr(file.path), StatementFileFormat::loadFromDb(db, file.formatId), false); // takes ~ 200 ms per file
		fileWatcher.addPath(qstr(file.path));
	}

	recalculateBalances(db); // takes ~ 100 ms
	emit newStatementsImported();
}

void StatementReader::stopWatchingFiles() {
	qLog() << "stop watching all files";
	fileWatcher.removePaths(fileWatcher.files());
}

void StatementReader::stopWatchingFile(cqstring filename) {
	qLog() << "stop watching file " << filename;
	fileWatcher.removePath(filename);

	db::File f;
	(*db)(sqlpp::update(f).set(f.watch = false).where(f.path == str(filename)));
}

void StatementReader::addFile(cqstring filename, const StatementFileFormat& format, bool watch) {
	db::File f;

	// remember file for auto import on startup (and watching)
	auto fs = (*db)(select(f.id, f.watch).from(f).where(f.path == str(filename)));
	if (fs.empty()) {
		(*db)(insert_into(f).set(f.path = str(filename),
									f.formatId = format.id,
									f.watch = watch));
		if (watch) {
			// new file that should be watched
			qLog() << "watching file " << filename;
			fileWatcher.addPath(filename);
		}
	} else {
		(*db)(sqlpp::update(f).set(f.formatId = format.id,
								f.watch = watch
								).where(f.id == fs.front().id));
		if (watch) {
			// file should be watched, but isn't so far
			qLog() << "watching file " << filename;
			fileWatcher.addPath(filename);
		} else if (!watch) {
			// file shouldn't be watched, but might currently be
			stopWatchingFile(filename);
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

	// if both accounts of a transfer are own accounts the transfer is internal
	const auto ownAccountIds = AccountModel::ownAccountIds(db);
	auto isInternalTransfer = [&](int fromId, int toId) {
		return ownAccountIds.contains(fromId) && ownAccountIds.contains(toId);
	};

	try {
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
			if (date.date().year() < 1970) date = date.addYears(100); // QDateTime::fromString will read 06.07.15 as 06.07.1915
			if (!date.isValid() || date.date().year() < 1970 || date.date().year() > 2070) {
				throw std::runtime_error(QString("invalid transfer date detected (%s), year must be between 1970 and 2070, possibly wrong date format for %s").arg(date.toString("dd.MM.yyyy")).arg(val("date")).toStdString());
			}

			auto from = hasVal("senderId") ? Transfer::Acc(val("senderId").toInt(), "") : makeAccount(val("senderOwnerName"), val("senderIban"), val("senderBic"));
			auto to = hasVal("receiverId") ? Transfer::Acc(val("receiverId").toInt(), "") : makeAccount(val("receiverOwnerName"), val("receiverIban"), val("receiverBic"));
		
			int amount = util::parseCurrency(val("amount"));
			if (format.invertAmount) amount = -amount;
			
			assert_error(from.id >= 0 && to.id >= 0, "from %d, to: %d", from.id, to.id);
			assert_debug(from.id != to.id, "accounts are not allowed to match (lineNumber: %d, from: %s, to: %s)", lineNumber, cstr(from), cstr(to));

			Transfer t;
			if (hasVal("id")) {
				t = Transfer(val("id").toInt(), date, from, to, val("reference"), amount, val("reference"), val("checked") == "1", val("internal") == "1");
				t.note = val("note");
				insert(t);
			} else {
				t = Transfer(date, from, to, val("reference"), amount);
				t.note = val("note");
				t.checked = val("checked") == "1";
				t.internal = hasVal("internal") ? val("internal") == "1" : isInternalTransfer(from.id, to.id);
				updateOrAdd(t);
			}
			assert_error(t.id >= 0);

			if (hasVal("tags")) {
				TagHelper th(db);
				th.addTransferTags(TagHelper::splitTags(val("tags")), {t.id});
			}
		});
	} catch (const std::invalid_argument& e) {
		stopWatchingFile(filename);
		assert_error(false, "%s", e.what());
	} catch (const std::runtime_error& e) {
		stopWatchingFile(filename);
		assert_warning(false, "%s", e.what());
	}

	if (emitSignal) { 
		recalculateBalances(db);
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

void StatementReader::recalculateBalances(Db db) {
	db::Account acc;
	db::Transfer tr;
	
	// for internal transfer we might have two transfer entries (one from each statement file of the two accounts)
	// therefor we will remember the hash of each internal transfer and exclude the second transfer from the calculation
	// QSet<QString> seenTransfers;
	std::unordered_multiset<std::string> seenTransfersFrom;
	std::unordered_multiset<std::string> seenTransfersTo;
	
	QHash<int, int> balances;
	for (const auto& t : (*db)(select(tr.date, tr.fromId, tr.toId, tr.amount, tr.internal).from(tr).where(true))) {
		bool addFrom = true;
		bool addTo = true;
		if (t.internal) {
			auto it = seenTransfersFrom.find(util::internalTransferHash(t.fromId, - t.amount, t.date));
			if (it != seenTransfersFrom.end()) {
				seenTransfersFrom.erase(it);
				addFrom = false;
			} else {
				seenTransfersTo.insert(util::internalTransferHash(t.fromId, t.amount, t.date));
			}

			it = seenTransfersTo.find(util::internalTransferHash(t.toId, - t.amount, t.date));
			if (it != seenTransfersTo.end()) {
				seenTransfersTo.erase(it);
				addTo = false;
			} else {
				seenTransfersFrom.insert(util::internalTransferHash(t.toId, t.amount, t.date));
			}
		}
		if (addFrom) balances[t.fromId] += t.amount;
		if (addTo) balances[t.toId] -= t.amount;
	}
	for (auto it = balances.begin(); it != balances.end(); it++) {
		(*db)(sqlpp::update(acc).set(acc.balance = acc.initialBalance + it.value()).where(acc.id == it.key()));
	}
}

Transfer::Acc StatementReader::makeAccount(cqstring owner, cqstring iban, cqstring bic) {
	Account a(owner, iban, bic);
	findOrAdd(a);
	assert_error(a.id >= 0);
	return Transfer::Acc(a.id, a.name, a.isOwn);
}

int StatementReader::find(Account& account) {
	db::Account acc;
	auto accsSql = dynamic_select(*(db.get()), acc.id, acc.isOwn).from(acc).dynamic_where();

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
		account.isOwn = accs.front().isOwn;
		return id;
	}

	return -1;
}

int StatementReader::find(Transfer& transfer) {
	db::Transfer tr;
	auto trs = (*db)(select(tr.id).from(tr).where(tr.date == transfer.dateMs() and
											tr.fromId == transfer.from.id and
											tr.toId == transfer.to.id and
											tr.reference == str(transfer.reference) and // TODO change to not empty and reference same or amount same
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

int StatementReader::updateOrAdd(Transfer& transfer) {
	int id = find(transfer);
	if (id == -1) {
		id = add(transfer);
	} else {
		update(transfer);
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
	qDebug() << "added " << account;
	return id;
}

int StatementReader::add(Transfer& transfer) {
	db::Transfer tr;
	int id = (*db)(insert_into(tr).set(tr.date = transfer.dateMs(),
										tr.fromId = transfer.from.id,
										tr.toId = transfer.to.id,
										tr.reference = str(transfer.reference),
										tr.amount = transfer.amount,
										tr.note = str(transfer.note),
										tr.checked = transfer.checked,
										tr.internal = transfer.internal
										));
	assert_error(id >= 0);
	transfer.id = id;
	qDebug() << "added " << transfer;
	return id;
}

void StatementReader::update(const Transfer& transfer) {
	db::Transfer tr;
	assert_error(transfer.id > 0);
	(*db)(sqlpp::update(tr).set(tr.date = transfer.dateMs(),
								tr.fromId = transfer.from.id,
								tr.toId = transfer.to.id,
								tr.reference = str(transfer.reference),
								tr.amount = transfer.amount,
								tr.checked = tr.checked || transfer.checked,
								tr.internal = tr.internal || transfer.internal
							).where(tr.id == transfer.id));
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
	qDebug() << "inserted " << account;
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
										tr.checked = transfer.checked,
										tr.internal = transfer.internal
										));

	qDebug() << "inserted " << transfer;
}
