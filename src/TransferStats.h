#ifndef TRANSFER_STATS_H
#define TRANSFER_STATS_H

#include <QObject>

class TransferStats : public QObject {
	Q_OBJECT
public:
	int revenues;
	int expenses;
	int internal;

	int revenuesCount;
	int expensesCount;
	int internalCount;
	
	TransferStats();

	int profit() const { return revenues + expenses; }

	int transferCount() const { return revenuesCount + expensesCount + internalCount; }

public slots:
	void reset();
	void add(int val, bool isInternal);
};

#endif // TRANSFER_STATS_H
