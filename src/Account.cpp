#include <sstream>
#include "Account.h"

Account::Account() :
	id(-1)
{}

Account::Account(int id, bool isOwn, string name, string owner, string iban, string bic, string accountNumber, string branchCode) :
	id(id), isOwn(isOwn), name(name), owner(owner), iban(iban), bic(bic), accountNumber(accountNumber), bankCode(branchCode)
{
}

Account::Account(string owner, string iban, string bic) :
	id(-1), isOwn(false), name(owner), owner(owner), iban(iban), bic(bic), accountNumber(""), bankCode("")
{
	if (!isSpecial()) {
		if (isValidIban(iban)) {
			assert(iban.length() >= 8);
			assert(iban.length() <= 30);
		} else {
			accountNumber = iban;
			this->iban = "";
		}

		if (!isValidBic(bic)) {
			bankCode = bic;
			this->bic = "";
		}
	}

	if (accountNumber.isEmpty() && !bankCode.isEmpty()) {
		accountNumber = bankCode;
		name = "Bank";
	}
}

bool Account::isSpecial() const {
	return iban.length() > 1 && iban[0] == QChar('#');
}

bool Account::operator==(const Account& acc) const {
	int x = 0;
	if (!iban.isEmpty() && !acc.iban.isEmpty()) {
		x++;
		if (iban != acc.iban) {
			return false;
		}
	}
	if (!bic.isEmpty() && !acc.bic.isEmpty()) {
		x++;
		if (bic != acc.bic) {
			return false;
		}
	}
	if (!accountNumber.isEmpty() && !acc.accountNumber.isEmpty()) {
		x++;
		if (accountNumber != acc.accountNumber) {
			return false;
		}
	}
	if (!bankCode.isEmpty() && !acc.bankCode.isEmpty()) {
		x++;
		if (bankCode != acc.bankCode) {
			return false;
		}
	}
	return x > 0;
}

bool Account::isValidIban(string iban) {
	return iban.length() >= 4 && iban[0].isLetter() && iban[1].isLetter() && iban[2].isNumber() && iban[3].isNumber();
}

bool Account::isValidBic(string bic) {
	if (bic.length() < 8 || bic.length() > 11) {
		return false;
	}
	assert(bic.length() >= 6);
	for (int i = 0; i < 6; i++) {
		if (!bic[i].isLetter()) {
			return false;
		}
	}
	return true;
}

std::ostream& operator<<(std::ostream& os, const Account& a) {
	os << "Account(" << a.id << ", "<< a.accountNumber << ", "<< a.bankCode << ", "<< a.iban << ", "<< a.bic << ")";
	return os;
}
