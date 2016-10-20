#ifndef TRANSFER_PROXY_MODEL_H
#define TRANSFER_PROXY_MODEL_H

#include <unordered_set>
#include <QDateTime>
#include <QSortFilterProxyModel>
#include <date.h>
#include "../sql.h"
#include "Transfer.h"

class TransferProxyModel : public QSortFilterProxyModel {
	Q_OBJECT

public:
	TransferProxyModel(Db db, QObject* parent = 0);

public slots:
	void setStartDate(const QDate& startDate);
	void setStartDate(const date::year_month_day& startDate);
	void setEndDate(const QDate& endDate);
	void setEndDate(const date::year_month_day& endDate);
	void setFilterText(const QString& filterText);
	void resetStatsAndInvalidateFilter();

signals:
	void resetStats();
	void addToStats(int transferId) const;
	void removeFromStats(int transferId) const;

protected:
	Db db;

	date::year_month_day startDate;
	date::year_month_day endDate;

	QString txtFrom;
	QString txtTo;
	QString txtRef;
	QString txtAmount;
	QString txtNote;
	int trChecked;
	int trTagged;
	int trInternal;
	std::unordered_set<int> txtTags;
	QString txtRest;

	const Transfer& get(int sourceRow) const;

	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
	bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
};

#endif // TRANSFER_PROXY_MODEL_H
