#include "TransferStats.h"

#include <cmath>

TransferStats::TransferStats() :
	revenuesSum(0),
	expensesSum(0),
	internalSum(0),
	statValues()
{}

std::vector<int> TransferStats::includedTransferIds() const {
	std::vector<int> ids;
	for (const auto& kv : statValues) {
		ids.push_back(kv.first);
	}
	return ids;
}


void TransferStats::clear() {
	revenuesSum = 0;
	expensesSum = 0;
	internalSum = 0;
	statValues.clear();
}

void TransferStats::add(const Transfer& transfer, bool isInternal) {
	auto it = statValues.find(transfer.id);
	if (it != statValues.end()) {
		// remove old stat value
		remove((*it).second);
	}

	auto& stat = statValues[transfer.id];
	stat.internal = isInternal;
	stat.amount = transfer.value.amount; // TODO: currency?
	add(stat);
}

void TransferStats::remove(const Transfer& transfer, bool isInternal) {
	auto it = statValues.find(transfer.id);
	if (it != statValues.end()) {
		remove((*it).second);
		statValues.erase(it);
	}
}

void TransferStats::add(const StatValue& stat) {
	if (stat.internal) {
		internalSum += std::abs(stat.amount);
	} else if (stat.amount < 0) {
		expensesSum += stat.amount;
	} else {
		revenuesSum += stat.amount;
	}
}

void TransferStats::remove(const StatValue& stat) {
	if (stat.internal) {
		internalSum -= std::abs(stat.amount);
	} else if (stat.amount < 0) {
		expensesSum -= stat.amount;
	} else {
		revenuesSum -= stat.amount;
	}
}
