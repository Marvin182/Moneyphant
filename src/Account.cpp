#include "Account.h"

#include <iostream>

Account::Account() :
	id(-1)
{}

Account::Account(cqstring owner, cqstring iban, cqstring bic) :
	id(-1), isOwn(false), name(owner), owner(owner), iban(iban), bic(bic), accountNumber(""), bankCode("")
{
	if (!isSpecial()) {
		if (isValidIban(iban)) {
			assert_debug(iban.length() >= 8);
			assert_debug(iban.length() <= 30);
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

Account::Account(int id, bool isOwn, cqstring name, cqstring owner, cqstring iban, cqstring bic, cqstring accountNumber, cqstring branchCode) :
	id(id),
	isOwn(isOwn),
	name(name),
	owner(owner),
	iban(iban),
	bic(bic),
	accountNumber(accountNumber),
	bankCode(branchCode)
{}

bool Account::isSpecial() const {
	return iban.length() > 1 && iban[0] == QChar('#');
}

Account::operator QString() const {
	return QString("Account(%1, %2, %3, %4, %5, %6)").arg(id).arg(name).arg(accountNumber).arg(bankCode).arg(iban).arg(bic);
}

bool Account::operator==(const Account& acc) const {
	bool x = false;
	if (!iban.isEmpty() && !acc.iban.isEmpty()) {
		x = true;
		if (iban != acc.iban) {
			return false;
		}
	}
	if (!bic.isEmpty() && !acc.bic.isEmpty()) {
		x = true;
		if (bic != acc.bic) {
			return false;
		}
	}
	if (!accountNumber.isEmpty() && !acc.accountNumber.isEmpty()) {
		x = true;
		if (accountNumber != acc.accountNumber) {
			return false;
		}
	}
	if (!bankCode.isEmpty() && !acc.bankCode.isEmpty()) {
		x = true;
		if (bankCode != acc.bankCode) {
			return false;
		}
	}
	// if (!otherIdentifier.isEmpty() && !acc.otherIdentifier.isEmpty()) {
	// 	x = true;
	// 	if (otherIdentifier != acc.otherIdentifier) {
	// 		return false;
	// 	}
	// }
	return x;
}

bool Account::isValidIban(cqstring iban) {
	return iban.length() >= 4 && iban[0].isLetter() && iban[1].isLetter() && iban[2].isNumber() && iban[3].isNumber();
}

bool Account::isValidBic(cqstring bic) {
	if (bic.length() < 8 || bic.length() > 11) {
		return false;
	}
	assert_error(bic.length() >= 6);
	for (int i = 0; i < 6; i++) {
		if (!bic[i].isLetter()) {
			return false;
		}
	}
	return true;
}

std::ostream& operator<<(std::ostream& os, const Account& a) {
	os << "Account(" << a.id << ", "<< str(a.name) << ", "<< a.accountNumber << ", "<< a.bankCode << ", "<< a.iban << ", "<< a.bic << ")";
	return os;
}
