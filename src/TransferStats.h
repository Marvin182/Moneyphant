#ifndef TRANSFER_STATS_H
#define TRANSFER_STATS_H

#include "Transfer.h"
#include <unordered_map>
#include <QObject>

class TransferStats : public QObject {
	Q_OBJECT
public:
	
	TransferStats();

	int revenues() const { return revenuesSum; }
	int expenses() const { return expensesSum; }
	int internal() const { return internalSum; }
	int profit() const { return revenues() + expenses(); }

	std::vector<int> includedTransferIds() const;

public slots:
	void clear();
	void add(const Transfer& transfer, bool isInternal);
	void remove(const Transfer& transfer, bool isInternal);

signals:
	void changed();

protected:
	struct StatValue {
		int internal;
		int amount;
		StatValue() :
			internal(false),
			amount(0)
		{}
	};

	int revenuesSum;
	int expensesSum;
	int internalSum;

	std::unordered_map<int, StatValue> statValues;

	void add(const StatValue& stat);
	void remove(const StatValue& stat);
};

#endif // TRANSFER_STATS_H
