#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Global.h"
#include <QStringList>

struct Account {
public:
	int id;
	bool isOwn;
	QString name;
	QString owner;
	QString iban;
	QString bic;
	QString accountNumber;
	QString bankCode;

	Account();
	Account(string owner, string iban, string bic);
	Account(int id, bool isOwn, string name, string owner, string iban, string bic, string accountNumber, string bankCode);

	bool isSpecial() const;

	bool operator==(const Account& acc) const;
	bool operator!=(const Account& acc) const { return !(*this == acc); }

private:
	static bool isValidIban(string iban);
	static bool isValidBic(string bic);
};

std::ostream& operator<<(std::ostream& os, const Account& a);

#endif // ACCOUNT_H
