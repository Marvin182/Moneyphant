#ifndef TRANSFER_PROXY_MODEL_H
#define TRANSFER_PROXY_MODEL_H

#include "mr/common.h"
#include "sql.h"
#include "Transfer.h"
#include <vector>
#include <QDateTime>
#include "QSortFilterProxyModel.h"

class TransferProxyModel : public QSortFilterProxyModel {
	Q_OBJECT

public:
	TransferProxyModel(Db db, QObject* parent = 0);

public slots:
	void setStartDate(const QDateTime& startDate);
	void setEndDate(const QDateTime& endDate);
	void setFilterText(const QString& filterText);
	void resetStatsAndInvalidateFilter();

signals:
	void resetStats();
	void addToStats(int transferId) const;
	void removeFromStats(int transferId) const;

protected:
	Db db;

	QDateTime startDate;
	QDateTime endDate;

	QString txtFrom;
	QString txtTo;
	QString txtRef;
	QString txtAmount;
	QString txtNote;
	int trChecked;
	std::vector<int> txtTags;
	QString txtRest;

	const Transfer& get(int sourceRow) const;

	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
	bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
};

#endif // TRANSFER_PROXY_MODEL_H
