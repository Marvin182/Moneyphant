#include "Updater.h"

#include <vector>
#include <algorithm>
#include <unordered_set>
#include <mr/common>
#include "version.h"
#include "Evolutions.h"
#include "StatementReader.h"
#include "model/Account.h"
#include <chrono>
#include <typeinfo>
#include <date.h>
using namespace date;

Updater::Updater(Db db, QSettings& settings) :
	db(db),
	settings(settings)
{}

void Updater::run() {
	int newBuild = version.build;
	int build = settings.value("updater/db_build_level", newBuild).toInt();
	assert_error(build <= newBuild, "build from settings: %d, new build: %d", build, newBuild);

	beforeEvolutions(build);
	Evolutions(db).run();
	afterEvolatuons(build);

	dbMaintenance();

	settings.setValue("updater/db_build_level", newBuild);
}

void Updater::beforeEvolutions(int build) {

}

void Updater::afterEvolatuons(int build) {
	db::Account acc;
	db::Transfer tr;

	if (build < 50) {
		qLog() << "DB update for build 50: normalize IBANs, BICs, account numbers and bank codes";
		// preprocessing for iban, bic, accountNumber and bankCode changed
		// apply changes to existing account => remove white spaces and transform to upper case
		std::vector<Account> accounts;
		for (const auto& a : (*db)(select(all_of(acc)).from(acc).unconditionally())) {
			accounts.push_back({(int)a.id, a.isOwn, (int)a.balance, qstr(a.name), qstr(a.owner), qstr(a.iban), qstr(a.bic), qstr(a.accountNumber), qstr(a.bankCode), (int)a.initialBalance});
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

	if (build <= 60) {
		qLog() << "DB update for build 60: mark transfers between own accounts as internal";
		std::unordered_set<int> ids;
		for (const auto& a : (*db)(select(acc.id).from(acc).where(acc.isOwn))) {
			ids.insert(a.id);
		}
		auto ownIds = sqlpp::value_list_t<std::unordered_set<int>>(ids);
		(*db)(sqlpp::update(tr).set(tr.internal = true).where(tr.fromId.in(ownIds) and tr.toId.in(ownIds)));
		StatementReader::recalculateBalances(db);
	}

	if (build < 83) {
		qLog() << "DB update for build 83: Set transfer.ymd fields from transfer.date";
		std::vector<std::pair<long, date::sys_days>> dates;
		for (const auto& t : (*db)(select(tr.id, tr.date, tr.fromId).from(tr).unconditionally())) {
			auto d = QDateTime::fromMSecsSinceEpoch(t.date, Qt::UTC).date();
			auto datum = date::year(d.year()) / d.month() / d.day();
			dates.push_back({t.id, datum});
		}
		for (const auto& p : dates) {
			(*db)(update(tr).set(tr.ymd = p.second).where(tr.id == p.first));
		}
	}

}

void Updater::dbMaintenance() {
	// db::Account acc;
	// db::Transfer tr;
	// db::Tag tg;
	// db::AccountTag accTg;
	// db::TransferTag trTg;

	// for (const auto& a : (*db)(select(acc.id, acc.name, count(acc.id)).from(acc.join(tr).on(tr.fromId == acc.id or tr.toId == acc.id)).group_by(acc.id).unconditionally().having(count(acc.id) == 0))) {
	// 	qLog() << QString("DB Maintenance: Delete Account(%1, %2), linked to %3 transfers").arg(a.id).arg(qstr(a.name)).arg(a.count);
	// }

	
	// db::Tag tg;
	// db::TransferTag trTg;
	// db::AccountTag accTg;

	// qLog() << "here";
	// for (const auto& t : (*db)(select(tg.id, tg.name, count(tg.id)).from(tg.join(trTg).on(trTg.tagId == tg.id)).group_by(tg.id).unconditionally())) {
	// 	qLog().space() << t.id << qstr(t.name) << t.count;
	// }
}
