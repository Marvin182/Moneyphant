#include "StatementReader.h"
#include "db.h"
#include <QDir>

StatementReader::StatementReader(Db db) :
	db(db)
{}

void StatementReader::importMissingStatementFiles(string folder) {
	auto files = QDir(folder).entryList(QStringList("*.csv"));
	assert(!files.empty() && "no statements found");
	for (auto& file : files) {
		importMissingStatementsCsv(folder + file);
	}
}

void StatementReader::importMissingStatementsCsv(string path) {
	// open file
	QFile file(path);
	assert(file.exists() && "statement file dosen't exist.");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		assert(false && "Could not open statement file");
	}
	assert(!file.atEnd() && "file is empty");

	// choose parsing mode depending on the number of fields in the first line
	auto header = file.readLine();
	auto headerSize = header.split(';').length();
	switch (headerSize) {
		case 11:
			parseMBSStatements(file);
			break;
		case 16:
			parseMastercardStatements(file);
			break;
		default:
			assert(false && "unknown statement header");
	}
	assert(file.atEnd() && "parsing did not parse whole file");
}

void StatementReader::parseMBSStatements(QFile& file) {
	while (!file.atEnd()) {
		auto line = QString(file.readLine());
		auto fields = line.split(';');
		assert(fields.size() == 11);
		for (auto& f : fields) {
			f = f.trimmed();
			if (f.length() > 1) {
				assert(f[0] == '"' && f[f.length() - 1] == '"');
				f = QStringRef(&f, 1, f.length() - 2).toString();
			}
		}

		auto from = acc("", fields[0], "");
		auto to = acc(fields[5], fields[6], fields[7]);
		assert(from.id >= 0 && to.id >= 0);
		assert(from.id != to.id); // accounts are not allowed to match
		Transfer t(fields[1], from, to, fields[4], fields[8]);
		findOrAdd(t);
		assert(t.id >= 0);
	}
}


void StatementReader::parseMastercardStatements(QFile& file) {
	/*
		0
		Umsatz getätigt von
		5490 0602 0100 7853

		1
		Belegdatum
		15.01.14

		2
		Buchungsdatum
		15.01.14

		3
		Originalbetrag
		-8,95

		4
		Originalwährung
		EUR

		5
		Umrechnungskurs
		1,00

		6
		Buchungsbetrag
		-8,95

		7
		Buchungswährung
		EUR

		8
		Transaktionsbeschreibung
		Amazon *Mktplce EU-DE WWW.AMAZON.DELU

		9
		Transaktionsbeschreibung Zusatz

		10
		Buchungsreferenz
		01500290551

		11
		Gebührenschlüssel

		12
		Länderkennzeichen

		13
		BAR-Entgelt+Buchungsreferenz

		14
		AEE+Buchungsreferenz

		15
		Abrechnungskennzeichen
		20140125
	*/
	while (!file.atEnd()) {
		auto line = QString(file.readLine());
		auto fields = line.split(';');
		assert(fields.size() == 16);
		for (auto& f : fields) {
			f = f.trimmed();
			if (f.length() > 1) {
				assert(f[0] == '"' && f[f.length() - 1] == '"');
				f = QStringRef(&f, 1, f.length() - 2).toString();
			}
		}

		auto from = acc("", fields[0], "");
		auto to = acc("Abbuchung Kreditkarte", "#11", "");
		assert(from.id >= 0 && to.id >= 0);
		assert(from.id != to.id); // accounts are not allowed to match
		Transfer t(fields[2], from, to, fields[8], fields[6]);
		findOrAdd(t);
		assert(t.id >= 0);
	}
}

Transfer::Acc StatementReader::acc(string owner, string iban, string bic) {
	Account a(owner, iban, bic);
	findOrAdd(a);
	return Transfer::Acc(a.id, a.name);
}


int StatementReader::find(Account& account) {
	db::Account acc;
	auto accsSql = dynamic_select(*(db.get()), acc.id).from(acc).dynamic_where();

	if (!account.owner.isEmpty()) {
		accsSql.where.add(acc.owner == str(account.owner));
	}
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
		assert(accs.begin() != accs.end());
		int id = accs.front().id;
		// TODO check if there is really just one matching entry
		account.id = id;
		assert(id >= 0);
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
		assert(trs.begin() != trs.end());
		int id = trs.front().id;
		// TODO check if there is really just one matching entry
		transfer.id = id;
		assert(id >= 0);
		return id;
	}
	return -1;
}

int StatementReader::add(Account& account) {
	db::Account acc;
	int id = db->run(insert_into(acc).set(acc.name = str(account.name),
										acc.owner = str(account.owner),
										acc.iban = str(account.iban),
										acc.bic = str(account.bic),
										acc.accountNumber = str(account.accountNumber),
										acc.bankCode = str(account.bankCode)));
	assert(id >= 0);
	account.id = id;
	return id;
}

int StatementReader::add(Transfer& transfer) {
	db::Transfer tr;
	int id = db->run(insert_into(tr).set(tr.date = transfer.dateMs(),
										tr.fromId = transfer.from.id,
										tr.toId = transfer.to.id,
										tr.reference = str(transfer.reference),
										tr.amount = transfer.amount));
	assert(id >= 0);
	transfer.id = id;
	return id;
}

int StatementReader::findOrAdd(Account& account) {
	int id = find(account);
	if (id == -1) {
		id = add(account);
	}
	assert(id >= 0);
	return id;
}

int StatementReader::findOrAdd(Transfer& transfer) {
	int id = find(transfer);
	if (id == -1) {
		id = add(transfer);
	}
	assert(id >= 0);
	return id;
}
