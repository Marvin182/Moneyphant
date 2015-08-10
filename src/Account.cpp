#include "Account.h"

#include <iostream>

Account::Account() :
	id(-1)
{}

Account::Account(cqstring owner, cqstring ibanOrAccountNumber, cqstring bicOrBankCode) :
	id(-1),
	isOwn(false),
	name(owner),
	owner(owner),
	iban(ibanOrAccountNumber),
	bic(bicOrBankCode),
	accountNumber(""),
	bankCode("")
{
	// remove whitespaces
	iban = iban.remove(QRegExp("\\s"));
	bic = bic.remove(QRegExp("\\s"));

	if (isSpecial()) {
		// iban field is used for special identifier (e-mail address or dummy account)
	} else if (isIban(iban)) {
		iban = iban.toUpper();
		assert_debug(iban.length() >= 8);
		assert_debug(iban.length() <= 30);
	} else {
		// iban field was used for accountNumber
		assert_error(accountNumber.isEmpty());
		accountNumber = iban;
		iban = "";
	}

	if (isBic(bic)) {
		bic = bic.toUpper();
	} else {
		assert_error(bankCode.isEmpty());
		bankCode = bic;
		bic = "";
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
	return isDummyAccount(iban) || isEmailAddress(iban);
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
	return x;
}

bool Account::isDummyAccount(cqstring s) {
	return QRegExp("#\\d+").exactMatch(s);
}

bool Account::isEmailAddress(cqstring s) {
	return QRegExp(".+@.+\\..+").exactMatch(s);
}

bool Account::isIban(cqstring s) {
	return s.length() >= 4 && s[0].isLetter() && s[1].isLetter() && s[2].isNumber() && s[3].isNumber();
}

bool Account::isBic(cqstring s) {
	/* BIC, as defined in ISO 9362.
	 * Format is BBBBCCLLbbb with
	 * BBBB: 4 letters, Institution Code or bank code.
	 * CC: 2 letters, ISO 3166-1 alpha-2 country code
	 * LL: 2 letters or digits, location code
	 * bbb: 3 letters or digits, optional branch code
	 */
	switch (s.length()) {
		case 8: return QRegExp("[a-zA-Z]{6}[a-zA-Z0-9]{2}").exactMatch(s);
		case 11: return QRegExp("[a-zA-Z]{6}[a-zA-Z0-9]{5}").exactMatch(s);
		default: return false;
	}
}

std::ostream& operator<<(std::ostream& os, const Account& a) {
	os << "Account(" << a.id << ", "<< str(a.name) << ", "<< a.accountNumber << ", "<< a.bankCode << ", "<< a.iban << ", "<< a.bic << ")";
	return os;
}
