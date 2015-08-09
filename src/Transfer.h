#ifndef OPERATION
#define OPERATION

#include <mr/common>
#include "Account.h"
#include <QDateTime>
#include <QStringList>

struct Transfer {
	struct Acc {
		int id;
		QString name;
		Acc() :
			id(-1)
		{}
		Acc(int id, cqstring name) :
			id(id),
			name(name)
		{}
		operator QString() const { return QString("Acc(%1, %2").arg(id).arg(name); }
		bool operator==(const Acc& other) const { return id == other.id; }
		bool operator<(const Acc& other) const { return name < other.name; }
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
