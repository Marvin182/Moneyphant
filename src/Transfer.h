#ifndef OPERATION
#define OPERATION

#include <QDateTime>
#include <QStringList>
#include "Global.h"
#include "Account.h"

struct Transfer {
	struct Acc {
		int id;
		QString name;
		//Account get() const;
		Acc(int id, string name) :
			id(id),
			name(name)
		{}
		bool operator==(const Acc& other) const {
			return id == other.id;
		}
		bool operator<(const Acc& other) const {
			return name < other.name;
		}
	};

	int id;
	QDateTime date;
	Acc from;
	Acc to;
	QString reference;
	int amount;
	QString note;
	bool checked;

	Transfer(int id, const QDateTime& date, const Acc& from, const Acc& to, string reference, int amount, string note, bool checked);
	Transfer(string dateStr, const Acc& from, const Acc& to, string reference, string amountStr);

	qint64 dateMs() const { return date.toMSecsSinceEpoch(); }
	QString dateStr() const { return date.toString("dd.MM.yyyy"); }

	QString amountStr() const { return QString::number(amount / 100.0, 'f', 2); }

	bool operator==(const Transfer& tr) const;
	bool operator<(const Transfer& tr) const;
};

std::ostream& operator<<(std::ostream& os, const Transfer::Acc& a);
std::ostream& operator<<(std::ostream& os, const Transfer& tr);


#endif // OPERATION
