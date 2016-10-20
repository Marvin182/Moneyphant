#ifndef CURRENCY_H
#define CURRENCY_H

#include <string>

class Currency {
public:
	const char* name;
	const char* isoCode;
	const char* symbol;

	Currency() = delete;
	Currency(const char* name, const char* isoCode, const char* symbol);
	// ~Currency();

	static Currency* getDefault();
	static Currency* get(const char* isoCode);
	static Currency* get(std::string isoCode) { return Currency::get(isoCode.c_str()); }

private:
	int hash() const;
};

#endif // CURRENCY_H
