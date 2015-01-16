#include <cassert>
#include <sstream>
#include <iostream>
#include "Transfer.h"

Transfer::Transfer() :
	id(-1)
{}

Transfer::Transfer(string dateStr, const Acc& from, const Acc& to, string reference, string amountStr) :
	id(-1),
	date(QDateTime::fromString(dateStr, "dd.MM.yy")),
	from(from),
	to(to),
	reference(reference),
	amount(0),
	note(""),
	checked(false)
{
	if (date.date().year() < 1970) {
		date = date.addYears(100);
	}
	assert(date.date().year() >= 1970 && date.date().year() < 2070);

	amount = QString(amountStr).replace(',', '.').toDouble() * 100;
	assert(amount != 0);
}

Transfer::Transfer(int id, const QDateTime& date, const Acc& from, const Acc& to, string reference, int amount, string note, bool checked) :
	id(id),
	date(date),
	from(from),
	to(to),
	reference(reference),
	amount(amount),
	note(note),
	checked(checked)
{}

bool Transfer::operator==(const Transfer& tr) const {
	return date == tr.date &&
			from == tr.from &&
			to == tr.to &&
			reference == tr.reference &&
			amount == tr.amount;
}

bool Transfer::operator<(const Transfer& tr) const {
	return date < tr.date;
}

std::ostream& operator<<(std::ostream& os, const Transfer::Acc& a) {
	os << "Acc(" << a.id << ", " << str(a.name) << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Transfer& tr) {
	os << "Transfer(" << str(tr.dateStr()) << ": " << tr.from << " -> " << tr.to << ": " << tr.amount << ", " << str(tr.reference) << ")";
	return os;
}

