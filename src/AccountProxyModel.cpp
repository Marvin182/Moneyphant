#include "AccountProxyModel.h"
#include "AccountModel.h"

AccountProxyModel::AccountProxyModel(Db db, QObject* parent) :
	QSortFilterProxyModel(parent),
	db(db),
	accIsOwn(0),
	accIsSpecial(0)
{}

void AccountProxyModel::setFilterText(const QString& filterText) {
	auto parts = filterText.split(' ', QString::SkipEmptyParts);

	accIsOwn = -1;
	accIsSpecial = -1;
	txtName = "";
	txtOwner = "";
	txtTags.clear();
	txtRest = "";

	for (const auto& p : parts) {
		assert_error(!p.isEmpty());
		if (p.startsWith("name:")) {
			txtName = p.right(p.length() - 5);
		} else if (p.startsWith("owner:")) {
			txtOwner = p.right(p.length() - 6);
		} else if (p == "own") {
			accIsOwn = 1;
		} else if (p == "special") {
			accIsSpecial = 1;
		} else {
			// search for tag
			db::Tag tag;
			auto t = (*db)(select(tag.id).from(tag).where(tag.name == str(p)));
			if (!t.empty()) {
				txtTags.push_back(t.front().id);
			} else {
				txtRest += txtRest.isEmpty() ? p : (" " + p);
			}
		}
	}
	invalidateFilter();
}

const Account& AccountProxyModel::get(int sourceRow) const {
	auto accModel = static_cast<AccountModel*>(sourceModel());
	return accModel->get(sourceRow);
}

bool AccountProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
	Q_UNUSED(sourceParent);
	const auto& account = get(sourceRow);

	bool accepted = true;
	if (!txtName.isEmpty() && !fuzzyMatch(account.name, txtName)) {
		accepted = false;
	} else if (!txtOwner.isEmpty() && !fuzzyMatch(account.owner, txtOwner)) {
		accepted = false;
	} else if (accIsOwn == 1 && !account.isOwn) {
		accepted = false;
	} else if (accIsSpecial == 1 && !account.isSpecial()) {
		accepted = false;
	} else if (!txtTags.empty()) {
		db::AccountTag accTag;
		for (int tagId : txtTags) {
			bool hasTag = (*db)(select(count(accTag.tagId)).from(accTag).where(accTag.tagId == tagId and accTag.accountId == account.id)).front().count > 0;
			if (!hasTag) {
				accepted = false;
				break;
			}
		}
	} else if (!txtRest.isEmpty() && !fuzzyMatch(account.name + account.owner + account.iban + account.bic, txtRest)) {
		accepted = false;
	}

	return accepted;
}

bool AccountProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
	assert_error(left.column() == right.column(), "columns must match for comparison (left column: %d, right column: %d)", left.column(), right.column());
	const auto& a = get(left.row());
	const auto& b = get(right.row());

	switch (left.column()) {
		case 0: return b.isOwn;
		case 1: return a.name < b.name;
		case 2: return a.owner < b.owner;
		case 3: return a.iban < b.iban;
		case 4: return a.bic < b.bic;
		case 5: return a.accountNumber < b.accountNumber;
		case 6: return a.bankCode < b.bankCode;
		default: assert_error(false, "invalid column %d for left index (row: %d)", left.column(), left.row());
	}

	assert_unreachable();
	return false;
}
