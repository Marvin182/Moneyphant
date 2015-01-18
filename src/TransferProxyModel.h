#ifndef TRANSFER_PROXY_MODEL_H
#define TRANSFER_PROXY_MODEL_H

#include "Global.h"
#include "Transfer.h"
#include <vector>
#include <QDateTime>
#include "QSortFilterProxyModel.h"

class TransferProxyModel : public QSortFilterProxyModel {
	Q_OBJECT

public:
	TransferProxyModel(Db db, QObject* parent = 0);

public slots:
	void setFromAccountId(int fromAccountId);
	void setStartDate(const QDateTime& startDate);
	void setEndDate(const QDateTime& endDate);
	void setTags(const std::vector<int>& tags);

signals:
	void resetStats();
	void addToStats(int transferId) const;

protected:
	Db db;

	int fromAccountId;
	QDateTime startDate;
	QDateTime endDate;
	std::vector<int> tags;

	const Transfer& get(int sourceRow) const;

	void resetStatsAndInvalidateFilter();

	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
	bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
};

#endif // TRANSFER_PROXY_MODEL_H
