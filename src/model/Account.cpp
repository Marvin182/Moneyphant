#include "Account.h"

Account::Account() :
	id(-1)
{}

Account::Account(cqstring owner, cqstring ibanOrAccountNumber, cqstring bicOrBankCode) :
	id(-1),
	isOwn(false),
	balance(0),
	name(owner),
	owner(owner),
	iban(""),
	bic(""),
	accountNumber(""),
	bankCode(""),
	initialBalance(0)
{
	setIban(ibanOrAccountNumber);
	setBic(bicOrBankCode);
	if (isEmailAddress(iban) && name.isEmpty()) name = iban;
}

Account::Account(int id, bool isOwn, int balance, cqstring name, cqstring owner, cqstring iban, cqstring bic, cqstring accountNumber, cqstring branchCode, int initialBalance) :
	id(id),
	isOwn(isOwn),
	balance(balance),
	name(name),
	owner(owner),
	iban(iban),
	bic(bic),
	accountNumber(accountNumber),
	bankCode(branchCode),
	initialBalance(initialBalance)
{}

bool Account::isSpecial() const {
	return isDummyAccount(iban) || isEmailAddress(iban);
}

void Account::setIban(QString s) {
	if (isDummyAccount(s) || isEmailAddress(s)) {
		iban = s;
		return;
	}
	s = s.remove(QRegExp("\\s")).toUpper();
	if (isIban(s)) {
		iban = s;
	} else {
		accountNumber = s;
	}
}

void Account::setBic(QString s) {
	s = s.remove(QRegExp("\\s")).toUpper();
	if (isBic(s)) {
		bic = s;
	} else {
		bankCode = s;
	}
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

bool Account::isDummyAccount(cqstring iban) {
	return QRegExp("#\\d+.+").exactMatch(iban);
}

bool Account::isEmailAddress(cqstring iban) {
	return QRegExp(".+@.+\\..+").exactMatch(iban);
}

bool Account::isIban(cqstring iban) {
	return QRegExp("^[a-zA-Z]{2}\\d{2}.+").exactMatch(iban);
}

bool Account::isBic(cqstring bic) {
	/* BIC, as defined in ISO 9362.
	 * Format is BBBBCCLLbbb with
	 * BBBB: 4 letters, Institution Code or bank code.
	 * CC: 2 letters, ISO 3166-1 alpha-2 country code
	 * LL: 2 letters or digits, location code
	 * bbb: 3 letters or digits, optional branch code
	 */
	switch (bic.length()) {
		case 8: return QRegExp("[a-zA-Z]{6}[a-zA-Z0-9]{2}").exactMatch(bic);
		case 11: return QRegExp("[a-zA-Z]{6}[a-zA-Z0-9]{5}").exactMatch(bic);
		default: return false;
	}
}

std::ostream& operator<<(std::ostream& os, const Account& a) {
	os << "Account(" << a.id << ", "<< str(a.name) << ", "<< a.accountNumber << ", "<< a.bankCode << ", "<< a.iban << ", "<< a.bic << ")";
	return os;
}
