#include "TransferProxyModel.h"
#include "TransferModel.h"
#include "db.h"

TransferProxyModel::TransferProxyModel(Db db, QObject* parent) :
	QSortFilterProxyModel(parent),
	db(db),
	fromAccountId(-1),
	startDate(QDate(2014, 1, 1)),
	endDate(QDate(2014, 12, 31)),
	filterText()
{}

void TransferProxyModel::setFromAccountId(int fromAccountId) {
	this->fromAccountId = fromAccountId;
	resetStatsAndInvalidateFilter();
}

void TransferProxyModel::setStartDate(const QDateTime& startDate) {
	this->startDate = startDate;
	resetStatsAndInvalidateFilter();
}

void TransferProxyModel::setEndDate(const QDateTime& endDate) {
	this->endDate = endDate;
	resetStatsAndInvalidateFilter();
}

void TransferProxyModel::setFilterText(const QString& filterText) {
	this->filterText = filterText;
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
	} else if  (fromAccountId >= 0 && transfer.from.id != fromAccountId) {
		accepted = false;
	} else if (!filterText.isEmpty() && !fuzzyContains(transfer.from.name + transfer.to.name + transfer.reference, filterText)) {
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
	const auto& trLeft = get(left.row());
	const auto& trRight = get(right.row());

	assert(left.column() == right.column());
	switch (left.column()) {
		case 0: return trLeft.date < trRight.date;
		case 1: return trLeft.from.name < trRight.from.name;
		case 2: return trLeft.to.name < trRight.to.name;
		case 3: return trLeft.reference < trRight.reference;
		case 4: return trLeft.amount < trRight.amount;
		case 5: return trLeft.checked;
		default: assert(false && "invalid column");
	}
}
