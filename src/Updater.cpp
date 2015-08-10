#include "Updater.h"

#include <vector>
#include <mr/Version>
#include "Evolutions.h"
#include "Account.h"

Updater::Updater(Db db, QSettings& settings) :
	db(db),
	settings(settings)
{}

void Updater::run() {
	int version = settings.value("updater/version", mr::Version::app().asNumber()).toInt();

	beforeEvolutions(version);
	Evolutions(db).run();
	afterEvolatuons(version);

	settings.setValue("updater/version", mr::Version::app().asNumber());
}

void Updater::beforeEvolutions(int version) {

}

void Updater::afterEvolatuons(int version) {
	db::Account acc;

	if (version < 9964) {
		qLog() << "Updating to version 9964";
		// preprocessing for iban, bic, accountNumber and bankCode changed
		// remove white spaces
		std::vector<Account> accounts;
		for (const auto& a : (*db)(select(all_of(acc)).from(acc).where(true))) {
			accounts.push_back({(int)a.id, a.isOwn, qstr(a.name), qstr(a.owner), qstr(a.iban), qstr(a.bic), qstr(a.accountNumber), qstr(a.bankCode)});
		}

		QRegExp rgx("\\s");
		for (auto& a : accounts) {
			if (a.isSpecial()) continue;
			a.iban = a.iban.remove(rgx).toUpper();
			a.bic = a.bic.remove(rgx).toUpper();
			a.accountNumber = a.accountNumber.remove(rgx);
			a.bankCode = a.bankCode.remove(rgx);
		}

		for (const auto& a : accounts) {
			(*db)(update(acc).set(acc.iban = str(a.iban),
								acc.bic = str(a.bic),
								acc.accountNumber = str(a.accountNumber),
								acc.bankCode = str(a.bankCode)
				).where(acc.id == a.id));
		}
	}
}