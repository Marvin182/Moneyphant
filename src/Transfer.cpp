#include <cassert>
#include <sstream>
#include <iostream>
#include "Transfer.h"

Transfer::Transfer() :
	id(-1)
{}

Transfer::Transfer(QDateTime dateTime, const Acc& from, const Acc& to, cqstring reference, int amount) :
	id(-1),
	date(dateTime),
	from(from),
	to(to),
	reference(reference),
	amount(amount),
	note(""),
	checked(false),
	internal(false)
{
	assert_error(date.date().year() >= 1970 && date.date().year() < 2070, "year is %d", date.date().year());
	assert_warning(amount != 0);
}

Transfer::Transfer(int id, const QDateTime& date, const Acc& from, const Acc& to, cqstring reference, int amount, cqstring note, bool checked, bool internal) :
	id(id),
	date(date),
	from(from),
	to(to),
	reference(reference),
	amount(amount),
	note(note),
	checked(checked),
	internal(internal)
{}

Transfer::operator QString() const {
	return QString("Transfer(%1: %2 -> %3: %4, %5)").arg(dateStr()).arg(from.name).arg(to.name).arg(amount).arg(reference);
}

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
