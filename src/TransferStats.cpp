#include "TransferStats.h"

TransferStats::TransferStats() :
	revenues(0),
	expenses(0),
	internal(0),
	revenuesCount(0),
	expensesCount(0),
	internalCount(0)
{}

void TransferStats::reset() {
	revenues = 0;
	expenses = 0;
	internal = 0;
	revenuesCount = 0;
	expensesCount = 0;
	internalCount = 0;
}

void TransferStats::add(int val, bool isInternal) {
	if (isInternal) {
		internalCount++;
		internal += val;
	} else if (val >= 0) {
		revenuesCount++;
		revenues += val;
	} else {
		expensesCount++;
		expenses += val;
	}
}
