#ifndef OPERATION
#define OPERATION

#include <QDateTime>
#include <mr/common>
#include "Account.h"

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
	QDateTime date;
	Acc from;
	Acc to;
	QString reference;
	int amount;
	QString note;
	bool checked;
	bool internal;

	Transfer();
	Transfer(QDateTime dateTime, const Acc& from, const Acc& to, cqstring reference, int amount);
	Transfer(int id, const QDateTime& date, const Acc& from, const Acc& to, cqstring reference, int amount, cqstring note, bool checked, bool internal);

	qint64 dateMs() const { return date.toMSecsSinceEpoch(); }
	QString dateStr() const { return date.toString("dd.MM.yyyy"); }

	operator QString() const;
	bool operator==(const Transfer& tr) const;
	bool operator<(const Transfer& tr) const;
};

std::ostream& operator<<(std::ostream& os, const Transfer::Acc& a);
std::ostream& operator<<(std::ostream& os, const Transfer& tr);


#endif // OPERATION
