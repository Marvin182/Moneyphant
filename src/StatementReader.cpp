#include "StatementReader.h"
#include "sql.h"
#include <QDir>

StatementReader::StatementReader(Db db) :
	db(db)
{}

void StatementReader::importMissingStatementFiles(cqstring folder) {
	auto files = QDir(folder).entryList(QStringList("*.csv"));
	//assert_warning(!files.empty() && "no statements found");
	for (const auto& file : files) {
		importMissingStatementsCsv(folder + file);
	}
}

/*void StatementReader::importMissingStatements(cqstring path, db::Format format) {
	QFile file(path);
	assert_error(file.exists(), "statement file '%s' dosen't exist", cstr(path));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		assert_error(false, "Could not open statement file '%s'", cstr(path));
	}
	assert_error(!file.atEnd(), "statement file '%s' is empty", cstr(path));

	parseCsvFile(file, format, [&](const QStringList& fields, int lineNumber) {
		// auto from = acc("", fields[0], "");
		// auto to = acc("Abbuchung Kreditkarte", "#11", "");
		// assert_error(from.id >= 0 && to.id >= 0);
		// assert_error(from.id != to.id); // accounts are not allowed to match
		// Transfer t(fields[2], from, to, fields[8], fields[6]);
		// findOrAdd(t);
		// assert_error(t.id >= 0);
	});

	assert_error(file.atEnd(), "did not parse whole statement file '%s'", cstr(path));
	// file.close() // TODO
}*/

void StatementReader::importMissingStatementsCsv(cqstring path) {
	// open file
	QFile file(path);
	assert_error(file.exists(), "statement file '%s' dosen't exist", cstr(path));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		assert_error(false, "Could not open statement file '%s'", cstr(path));
	}
	assert_error(!file.atEnd(), "statement file '%s' is empty", cstr(path));

	// choose parsing mode depending on the number of fields in the first line
	auto header = file.readLine();
	assert_error(!header.isEmpty(), "empty header in statement file '%s'", cstr(path));
	auto headerSize = header.contains(';') ? header.split(';').length() : header.split(',').length();
	switch (headerSize) {
		case 7:
		case 8:
			parseAccountFile(file);
			break;
		case 11:
			parseMBSStatements(file);
			break;
		case 16:
			parseMastercardStatements(file);
			break;
		case 43:
			parsePayPalStatements(file);
			break;
		default:
			assert_error(false, "unknown statement header");
	}
	assert_error(file.atEnd(), "did not parse whole statement file '%s'", cstr(path));
}

void StatementReader::parseAccountFile(QFile& file) {
	while (!file.atEnd()) {
		auto line = QString(file.readLine());
		auto fields = line.split(';');
		if (fields.size() == 7) {
			Account a(-1, fields[0] == "1", fields[1], fields[2], fields[3], fields[4], fields[5], fields[6].trimmed());
			findOrAdd(a);
			assert_error(a.id >= 0);
		} else {
			assert_error(fields.size() == 8);
			Account a(fields[0].toInt(), fields[1] == "1", fields[2], fields[3], fields[4], fields[5], fields[6], fields[7].trimmed());
		}
	}
}

void StatementReader::parseMBSStatements(QFile& file) {
	while (!file.atEnd()) {
		auto line = QString(file.readLine());
		auto fields = line.split(';');
		assert_error(fields.size() == 11);
		for (auto& f : fields) {
			f = f.trimmed();
			if (f.length() > 1) {
				assert_error(f[0] == '"' && f[f.length() - 1] == '"');
				f = QStringRef(&f, 1, f.length() - 2).toString();
			}
		}

		auto from = acc("", fields[0], "");
		auto to = acc(fields[5], fields[6], fields[7]);
		assert_error(from.id >= 0 && to.id >= 0);
		if (from.id == to.id) {
			std::cout << from << " -> " << to << std::endl << line << std::endl;
		}

		assert_error(from.id != to.id); // accounts are not allowed to match
		Transfer t(fields[1], from, to, fields[4], fields[8]);
		findOrAdd(t);
		assert_error(t.id >= 0);
	}
}


void StatementReader::parseMastercardStatements(QFile& file) {
	/*
		Umsatz getätigt von: 5490 0602 0100 7853
		Belegdatum: 15.01.14
		Buchungsdatum: 15.01.14
		Originalbetrag: -8,95
		Originalwährung: EUR
		Umrechnungskurs: 1,00
		Buchungsbetrag: -8,95
		Buchungswährung: EUR
		Transaktionsbeschreibung: Amazon *Mktplce EU-DE WWW.AMAZON.DELU
		Transaktionsbeschreibung Zusatz: 
		Buchungsreferenz: 01500290551
		Gebührenschlüssel: 
		Länderkennzeichen: 
		BAR-Entgelt+Buchungsreferenz: 
		AEE+Buchungsreferenz: 
		Abrechnungskennzeichen: 20140125
	*/
	while (!file.atEnd()) {
		auto line = QString(file.readLine());
		auto fields = line.split(';');
		assert_error(fields.size() == 16);
		for (auto& f : fields) {
			f = f.trimmed();
			if (f.length() > 1) {
				assert_error(f[0] == '"' && f[f.length() - 1] == '"');
				f = QStringRef(&f, 1, f.length() - 2).toString();
			}
		}

		auto from = acc("", fields[0], "");
		auto to = acc("Abbuchung Kreditkarte", "#11", "");
		assert_error(from.id >= 0 && to.id >= 0);
		assert_error(from.id != to.id); // accounts are not allowed to match
		Transfer t(fields[2], from, to, fields[8], fields[6]);
		findOrAdd(t);
		assert_error(t.id >= 0);
	}
}

void StatementReader::parsePayPalStatements(QFile& file) {
	/*
		 0 Datum: 16.10.2014
		 1 Zeit: 17:43:24
		 2 Zeitzone: MESZ
		 3 Name: Spotify Finance Ltd
		 4 Art: Zahlung per Händlerabbuchung wurde gesendet.
		 5 Status: Abgeschlossen
		 6 Währung: EUR
		 7 Brutto: -9,99
		 8 Gebühr: 0,00
		 9 Netto: -9,99
		10 Von E-Mail-Adresse: marvin.ritter@freenet.de
		11 An E-Mail-Adresse: premium-gbp@spotify.com
		12 Transaktionscode: 5JK46607VS589472P
		13 Status der Gegenpartei: Nicht-US-verifiziert
		Adressstatus: 
		Verwendungszweck: 
		Artikelnummer: 
		Betrag für Versandkosten: 0,00
		Versicherungsbetrag: 
		Umsatzsteuer: 1,60
		Option 1 - Name: 
		Option 1 - Wert: 
		Option 2 - Name: 
		Option 2 - Wert: 
		Auktions-Site: 
		Käufer-ID: 
		Artikel-URL: 
		Angebotsende: 
		Vorgangs-Nr.: 
		Rechnungs-Nr.: 
		Txn-Referenzkennung: 
		Rechnungsnummer: 453386974019
		Individuelle Nummer: 
		Menge: 1
		Bestätigungsnummer: 0,00
		Guthaben: 
		Adresse: 
		Zusätzliche Angaben: 
		Ort: 
		Staat/Provinz/Region/Landkreis/Territorium/Präfektur/Republik: 
		PLZ: 
		Land: 
		Telefonnummer der Kontaktperson: 
	*/
	parseCsvFile(file, ",", "", [&](const QStringList& fields, int lineNumber) {
		assert_error(fields.length() == 43, "invalid line %d in '%s'", lineNumber, cstr(file.fileName()));
		auto from = acc("PayPal Konto", fields[10], "");
		auto to = acc(fields[3], fields[11], "");
	});
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

int StatementReader::add(Account& account) {
	assert_error(account.id == -1, "cannot add account that has already an id (id: %d, name: %s)", account.id, cstr(account.name));

	db::Account acc;
	int id = db->run(insert_into(acc).set(acc.name = str(account.name),
										acc.owner = str(account.owner),
										acc.iban = str(account.iban),
										acc.bic = str(account.bic),
										acc.accountNumber = str(account.accountNumber),
										acc.bankCode = str(account.bankCode)));
	assert_error(id >= 0);
	account.id = id;
	return id;
}

void StatementReader::insert(Account& account) {
	assert_error(account.id >= 0);
	
	db::Account acc;
	assert_error(db->run(select(count(acc.id)).from(acc).where(acc.id == account.id)).front().count == 0, "there exists already an account with id %d", account.id);
	db->run(insert_into(acc).set(acc.id = account.id,
										acc.name = str(account.name),
										acc.owner = str(account.owner),
										acc.iban = str(account.iban),
										acc.bic = str(account.bic),
										acc.accountNumber = str(account.accountNumber),
										acc.bankCode = str(account.bankCode)));
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
	return id;
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
