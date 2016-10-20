#include "CurrencyWithAmount.h"
#include <QLocale>
#include <mr/common>
#include "Currency.h"

CurrencyWithAmount::CurrencyWithAmount() :
	currency(Currency::getDefault()),
	amount(0)
{}

CurrencyWithAmount::CurrencyWithAmount(long long amount, Currency* currency) :
	currency(currency),
	amount(amount)
{
	assert_error(currency);
}

CurrencyWithAmount::operator QString() const {
	return QLocale().toCurrencyString(amount / 100.0, currency->symbol);
}

bool CurrencyWithAmount::operator==(const CurrencyWithAmount& other) const {
	// We can compare currency pointers because there should be only one currency object per Currency
	return currency == other.currency && amount == other.amount;
}

bool CurrencyWithAmount::operator<(const CurrencyWithAmount& other) const {
	// only compare by amount, we wouldn't know the correct conversation rate without a date
	return amount < other.amount;
}
