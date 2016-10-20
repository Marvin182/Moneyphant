#include "Transfer.h"

#include <iostream>
#include "../util.h"

Transfer::Transfer() :
	id(-1),
	ymd(date::year(1970)/1/1)
{}

Transfer::Transfer(date::year_month_day ymd, const Acc& from, const Acc& to, cqstring reference, CurrencyWithAmount value) :
	id(-1),
	ymd(ymd),
	from(from),
	to(to),
	reference(reference),
	value(value),
	note(""),
	checked(false),
	internal(false)
{
	// TODO
	// assert_error(date.date().year() >= 1970 && date.date().year() < 2070, "year is %d", date.date().year());
	assert_warning(value.amount != 0);
}

Transfer::Transfer(int id, const date::year_month_day& ymd, const Acc& from, const Acc& to, cqstring reference, CurrencyWithAmount value, cqstring note, bool checked, bool internal) :
	id(id),
	ymd(ymd),
	from(from),
	to(to),
	reference(reference),
	value(value),
	note(note),
	checked(checked),
	internal(internal)
{}

QString Transfer::dateStr(const char* format) const {
	return util::toQDate(ymd).toString(format);
}

Transfer::operator QString() const {
	return QString("Transfer(%1: %2 -> %3: %4, %5)").arg(dateStr()).arg(from.name).arg(to.name).arg(value.amount).arg(reference);
}

Transfer& Transfer::operator=(const Transfer& other) {
	this->id = other.id;
	this->ymd = other.ymd;
	this->from = other.from;
	this->to = other.to;
	this->reference = other.reference;
	this->value = other.value;
	this->note = other.note;
	this->checked = other.checked;
	this->internal = other.internal;
	return *this;
}

bool Transfer::operator==(const Transfer& other) const {
	return ymd == other.ymd &&
			from == other.from &&
			to == other.to &&
			reference == other.reference &&
			value == other.value;
}

bool Transfer::operator<(const Transfer& other) const {
	return ymd < other.ymd;
}

bool Transfer::Acc::operator<(const Transfer::Acc& other) const {
	if (isOwn) {
		return !other.isOwn || name < other.name;
	}
	return name < other.name;
}

std::ostream& operator<<(std::ostream& os, const Transfer::Acc& a) {
	os << (a.isOwn ? "OwnAcc" : "Acc") << "(" << a.id << ", " << str(a.name) << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Transfer& tr) {
	os << "Transfer(" << str(tr.dateStr()) << ": " << tr.from << " -> " << tr.to << ": " << tr.value.amount << ", " << str(tr.reference) << ")";
	return os;
}
