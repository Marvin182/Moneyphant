#include "TransferProxyModel.h"
#include "TransferModel.h"

TransferProxyModel::TransferProxyModel(Db db, QObject* parent) :
	QSortFilterProxyModel(parent),
	db(db),
	startDate(QDate(2014, 1, 1)),
	endDate(QDate(2014, 12, 31)),
	trChecked(0)
{}

void TransferProxyModel::setStartDate(const QDateTime& startDate) {
	this->startDate = startDate;
	resetStatsAndInvalidateFilter();
}

void TransferProxyModel::setEndDate(const QDateTime& endDate) {
	this->endDate = endDate;
	resetStatsAndInvalidateFilter();
}

void TransferProxyModel::setFilterText(const QString& filterText) {
	auto parts = filterText.split(' ', QString::SkipEmptyParts);

	txtFrom = "";
	txtTo = "";
	txtRef = "";
	txtAmount = "";
	txtNote = "";
	trChecked = 0;
	txtTags.clear();
	txtRest = "";

	for (const auto& p : parts) {
		assert_error(!p.isEmpty());
		if (p.startsWith("from:")) {
			txtFrom = p.right(p.length() - 5);
		} else if (p.startsWith("to:")) {
			txtTo = p.right(p.length() - 3);
		} else if (p[0] == '-' || p[0].isNumber()) {
			txtAmount = p;
		} else if (p == "checked") {
			trChecked = 1;
		} else if (p == "unchecked") {
			trChecked = -1;
		} else if (p.startsWith("ref:")) {
			txtRef = p.right(p.length() - 4);
		} else if (p.startsWith("note:")) {
			txtNote = p.right(p.length() - 5);
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

	resetStatsAndInvalidateFilter();
}

void TransferProxyModel::resetStatsAndInvalidateFilter() {
	emit resetStats();
	invalidateFilter();
}

const Transfer& TransferProxyModel::get(int sourceRow) const {
	auto trModel = static_cast<TransferModel*>(sourceModel());
	return trModel->get(sourceRow);
}

bool TransferProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
	Q_UNUSED(sourceParent);
	const auto& transfer = get(sourceRow);

	bool accepted = true;
	if (startDate.isValid() && transfer.date < startDate) {
		accepted = false;
	} else if (endDate.isValid() && transfer.date > endDate) {
		accepted = false;
	} else if (!txtFrom.isEmpty() && !fuzzyMatch(transfer.from.name, txtFrom)) {
		accepted = false;
	} else if (!txtTo.isEmpty() && !fuzzyMatch(transfer.to.name, txtTo)) {
		accepted = false;
	} else if (!txtRef.isEmpty() && !fuzzyMatch(transfer.reference, txtRef)) {
		accepted = false;
	} else if (!txtAmount.isEmpty() && !fuzzyMatch(currency(transfer.amount), txtAmount)) {
		accepted = false;
	} else if (!txtNote.isEmpty() && !fuzzyMatch(transfer.note, txtNote)) {
		accepted = false;
	} else if ((trChecked == 1 && !transfer.checked) || (trChecked == -1 && transfer.checked)) {
		accepted = false;
	} else if (!txtTags.empty()) {
		db::TransferTag trTag;
		db::AccountTag accTag;
		for (int tagId : txtTags) {
			bool hasTag = (*db)(select(count(trTag.tagId)).from(trTag).where(trTag.tagId == tagId and trTag.transferId == transfer.id)).front().count > 0;
			if (!hasTag) {
				hasTag = (*db)(select(count(accTag.tagId)).from(accTag).where(accTag.tagId == tagId
					and (accTag.accountId == transfer.from.id or accTag.accountId == transfer.to.id))).front().count > 0;
			}
			if (!hasTag) {
				accepted = false;
				break;
			}
		}
	} else if (!txtRest.isEmpty() && !fuzzyMatch(transfer.from.name + transfer.to.name + currency(transfer.amount) + transfer.reference, txtRest)) {
		accepted = false;
	}

	if (accepted) {
		emit addToStats(transfer.id);
	} else {
		emit removeFromStats(transfer.id);
	}
	
	return accepted;
}

bool TransferProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
	assert_error(left.column() == right.column(), "columns must match for comparison (left column: %d, right column: %d)", left.column(), right.column());
	const auto& trLeft = get(left.row());
	const auto& trRight = get(right.row());

	switch (left.column()) {
		case 0: return trLeft.date < trRight.date;
		case 1: return trLeft.from.name < trRight.from.name;
		case 2: return trLeft.to.name < trRight.to.name;
		case 3: return trLeft.reference < trRight.reference;
		case 4: return trLeft.amount < trRight.amount;
		case 5: return trLeft.checked;
		default: assert_error(false, "invalid column %d for left index (row: %d)", left.column(), left.row());
	}

	assert_unreachable();
	return false;
}
