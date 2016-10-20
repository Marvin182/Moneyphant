#ifndef TRANSFER_H
#define TRANSFER_H

#include <QDateTime>
#include <mr/common>
#include <date.h>
#include "Account.h"
#include "CurrencyWithAmount.h"

struct Transfer {
	struct Acc {
		int id;
		bool isOwn;
		QString name;
		Acc() :
			id(-1),
			isOwn(false)
		{}
		Acc(int id, cqstring name, bool isOwn = false) :
			id(id),
			isOwn(isOwn),
			name(name)
		{}
		operator QString() const { return QString("%1(%2, %3)").arg(isOwn ? "OwnAcc" : "Acc").arg(id).arg(name); }
		bool operator==(const Acc& other) const { return id == other.id; }
		bool operator<(const Acc& other) const;
	};

	int id;
	date::year_month_day ymd;
	Acc from;
	Acc to;
	QString reference;
	CurrencyWithAmount value;
	QString note;
	bool checked;
	bool internal;

	Transfer();
	Transfer(date::year_month_day ymd, const Acc& from, const Acc& to, cqstring reference, CurrencyWithAmount value);
	Transfer(int id, const date::year_month_day& ymd, const Acc& from, const Acc& to, cqstring reference, CurrencyWithAmount value, cqstring note, bool checked, bool internal);

	date::sys_days dayPoint() const { return date::sys_days{ymd}; }
	QString dateStr(const char* format = "dd.MM.yyyy") const;

	operator QString() const;
	Transfer& operator=(const Transfer& other);	
	bool operator==(const Transfer& other) const;
	bool operator<(const Transfer& other) const;
};

std::ostream& operator<<(std::ostream& os, const Transfer::Acc& a);
std::ostream& operator<<(std::ostream& os, const Transfer& tr);


#endif // TRANSFER_H
