#include "StatementReader.h"
#include "sql.h"
#include <QDir>
#include <QtDebug>

StatementReader::StatementReader(Db db) :
	db(db)
{}

void StatementReader::importStatementFile(cqstring filename, const StatementFileFormat& format) {
	qInfo() << "importing " << filename;
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
		assert_debug(from.id != to.id, "accounts are not allowed to match");

		if (hasVal("id")) {
			Transfer t(val("id").toInt(), date, from, to, val("reference"), amount, val("reference"), val("checked") == "1", val("internal") == "1");
			insert(t);
		} else {
			Transfer t(date, from, to, val("reference"), amount);
			findOrAdd(t);
			assert_error(t.id >= 0);
		}
	});
}

QStringList& StatementReader::addFieldsFromLineSuffix(QStringList& fields, const StatementFileFormat& format) {
	if (format.lineSuffix.isEmpty()) return fields;
	auto suffix = format.lineSuffix;
	if (suffix.startsWith(format.delimiter)) {
		suffix = suffix.right(suffix.length() - format.delimiter.length());
	}
	if (format.textQualifier.isEmpty()) return fields << suffix.split(format.delimiter);
	return fields << mr::string::splitAndTrim(suffix, format.delimiter, format.textQualifier);
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

	if (!account.iban.isEmpty()) {
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

	auto accs = db->run(accsSql);

	if (!accs.empty()) {
		assert_error(accs.begin() != accs.end());
		int id = accs.front().id;
		assert_error(id >= 0);
		account.id = id;
		return id;
	}

	return -1;
}

int StatementReader::find(Transfer& transfer) {
	db::Transfer tr;
	auto trs = db->run(select(tr.id).from(tr).where(tr.date == transfer.dateMs() and
											tr.fromId == transfer.from.id and
											tr.toId == transfer.to.id and
											tr.reference == str(transfer.reference) and
											tr.amount == transfer.amount));
	if (!trs.empty()) {
		assert_error(trs.begin() != trs.end());
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
	int id = db->run(insert_into(acc).set(acc.isOwn = account.isOwn,
										acc.name = str(account.name),
										acc.owner = str(account.owner),
										acc.iban = str(account.iban),
										acc.bic = str(account.bic),
										acc.accountNumber = str(account.accountNumber),
										acc.bankCode = str(account.bankCode)));
	assert_error(id >= 0);
	account.id = id;
	qInfo() << "added " << account;
	return id;
}

int StatementReader::add(Transfer& transfer) {
	db::Transfer tr;
	int id = db->run(insert_into(tr).set(tr.date = transfer.dateMs(),
										tr.fromId = transfer.from.id,
										tr.toId = transfer.to.id,
										tr.reference = str(transfer.reference),
										tr.amount = transfer.amount));
	assert_error(id >= 0);
	transfer.id = id;
	qInfo() << "added " << transfer;
	return id;
}

void StatementReader::insert(const Account& account) {
	db::Account acc;
	assert_error(account.id >= 0);
	assert_error(db->run(select(count(acc.id)).from(acc).where(acc.id == account.id)).front().count == 0, "there exists already an account with id %d", account.id);
	
	db->run(insert_into(acc).set(acc.id = account.id,
								acc.isOwn = account.isOwn,
								acc.name = str(account.name),
								acc.owner = str(account.owner),
								acc.iban = str(account.iban),
								acc.bic = str(account.bic),
								acc.accountNumber = str(account.accountNumber),
								acc.bankCode = str(account.bankCode)
								));
	qInfo() << "inserted " << account;
}

void StatementReader::insert(const Transfer& transfer) {
	db::Transfer tr;
	assert_error(transfer.id >= 0);
	assert_error(db->run(select(count(tr.id)).from(tr).where(tr.id == transfer.id)).front().count == 0, "there exists already a transfer with id %d", transfer.id);

	db->run(insert_into(tr).set(tr.id = transfer.id,
										tr.date = transfer.dateMs(),
										tr.fromId = transfer.from.id,
										tr.toId = transfer.to.id,
										tr.reference = str(transfer.reference),
										tr.amount = transfer.amount,
										tr.note = str(transfer.note),
										tr.checked = transfer.checked
										));

	qInfo() << "inserted " << transfer;
}
