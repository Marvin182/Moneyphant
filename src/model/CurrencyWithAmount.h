#ifndef CURRENCY_WITH_AMOUNT_H
#define CURRENCY_WITH_AMOUNT_H

#include <QString>

class Currency;

class CurrencyWithAmount {
public:
	Currency* currency;
	long long amount;

        CurrencyWithAmount();
        CurrencyWithAmount(long long amount, Currency* currency);

	operator QString() const;
	bool operator==(const CurrencyWithAmount& other) const;
	bool operator<(const CurrencyWithAmount& other) const;
};

#endif // CURRENCY_WITH_AMOUNT_H
