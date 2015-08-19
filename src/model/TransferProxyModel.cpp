#include "TransferProxyModel.h"

#include <mr/common>
#include "TransferModel.h"
#include "../util.h"

TransferProxyModel::TransferProxyModel(Db db, QObject* parent) :
	QSortFilterProxyModel(parent),
	db(db),
	startDate(QDate(2014, 1, 1)),
	endDate(QDate(2014, 12, 31))
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
	txtFrom = "";
	txtTo = "";
	txtRef = "";
	txtAmount = "";
	txtNote = "";
	trChecked = 0;
	trTagged = 0;
	trInternal = 0;
	txtTags.clear();
	txtRest = "";

	QStringList parts;
	QString filters = QString(filterText).replace(":", ": ");
	try {
		parts = mr::split(filters, " ", "\"", QString::SkipEmptyParts);
	} catch (std::invalid_argument &e) {
		// user has not finished typing
		return;
	}

	int i = 0;
	auto nextPart = [&]() { return i < parts.length() ? parts[i++] : ""; };
	while (i < parts.length()) {
		auto p = nextPart();
		assert_error(!p.isEmpty());
		if (p == "from:") {
			txtFrom = nextPart();
		} else if (p == "to:") {
			txtTo = nextPart();
		} else if (p[0] == '-' || p[0].isNumber()) {
			txtAmount = p;
		} else if (p == "checked") {
			trChecked = 1;
		} else if (p == "unchecked") {
			trChecked = -1;
		} else if (p == "tagged") {
			trTagged = 1;
		} else if (p == "untagged") {
			trTagged = -1;
		} else if (p == "internal") {
			trInternal = 1;
		} else if (p == "ref:" || p == "reference:") {
			txtRef = nextPart();
		} else if (p == "note:") {
			txtNote = nextPart();
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

	auto contains = [&](cqstring text, cqstring needle) {
		return needle.contains(' ') ? text.contains(needle) : fuzzyMatch(text, needle);
	};

	db::TransferTag trTag;
	db::AccountTag accTag;

	// TODO: cache tag stuff until next user change

	bool accepted = true;
	if (startDate.isValid() && transfer.date < startDate) {
		accepted = false;
	} else if (endDate.isValid() && transfer.date > endDate) {
		accepted = false;
	} else if (!txtFrom.isEmpty() && !contains(transfer.from.name, txtFrom)) {
		accepted = false;
	} else if (!txtTo.isEmpty() && !contains(transfer.to.name, txtTo)) {
		accepted = false;
	} else if (!txtRef.isEmpty() && !contains(transfer.reference, txtRef)) {
		accepted = false;
	} else if (!txtAmount.isEmpty() && !util::formatCurrency(transfer.amount).contains(txtAmount)) {
		accepted = false;
	} else if (!txtNote.isEmpty() && !contains(transfer.note, txtNote)) {
		accepted = false;
	} else if ((trChecked == 1 && !transfer.checked) || (trChecked == -1 && transfer.checked)) {
		accepted = false;
	} else if (trTagged != 0) {
			bool tagged = (*db)(select(count(trTag.tagId)).from(trTag).where(trTag.transferId == transfer.id)).front().count > 0;
			tagged = tagged || (*db)(select(count(accTag.tagId)).from(accTag).where(accTag.accountId == transfer.from.id or accTag.accountId == transfer.to.id)).front().count > 0;
			if ((trTagged == 1 && !tagged) || (trTagged == -1 && tagged)) {
				accepted = false;
			}
	} else if (trInternal == 1 && !transfer.internal) {
		accepted = false;
	} else if (!txtTags.empty()) {
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
	} else if (!txtRest.isEmpty() && !fuzzyMatch(transfer.from.name + transfer.to.name + util::formatCurrency(transfer.amount) + transfer.reference, txtRest)) {
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
