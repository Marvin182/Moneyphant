
MoneyAmount::operator QString() const {
	return QLocale().toCurrencyString(value / 100.0, currency.symbol);
}
