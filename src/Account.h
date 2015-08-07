#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <mr/common>
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
	Account(cqstring owner, cqstring iban, cqstring bic);
	Account(int id, bool isOwn, cqstring name, cqstring owner, cqstring iban, cqstring bic, cqstring accountNumber, cqstring bankCode);

	bool isSpecial() const;

	operator QString() const;
	bool operator==(const Account& acc) const;
	bool operator!=(const Account& acc) const { return !(*this == acc); }

private:
	static bool isValidIban(cqstring iban);
	static bool isValidBic(cqstring bic);
};

std::ostream& operator<<(std::ostream& os, const Account& a);

#endif // ACCOUNT_H
