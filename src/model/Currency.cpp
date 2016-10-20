#include "Currency.h"
#include <array>
#include <unordered_set>
#include <cstring>
#include <mr/common>

std::unordered_set<int> existingCurrencies;
extern std::array<Currency, 160> allCurrencies;

// Currency::Currency() :
// 	name("Invalid Currency"),
// 	isoCode("ZZZ"),
// 	symbol("")
// {}

Currency::Currency(const char* name, const char* isoCode, const char* symbol) :
	name(name),
	isoCode(isoCode),
	symbol(symbol)
{
	auto h = hash();
	assert_error(existingCurrencies.count(h) == 0);
	existingCurrencies.insert(h);
}

// Currency::~Currency() {
// 	auto h = hash();
// 	assert_debug(existingCurrencies.count(h) == 1);
// 	existingCurrencies.erase(h);
// }

Currency* Currency::getDefault() {
	return &allCurrencies[0];
}

Currency* Currency::get(const char* isoCode) {
	for (auto& cur : allCurrencies) {
		if (strcmp(cur.isoCode, isoCode) == 0) return &cur;
	}
	return &allCurrencies.back();
}

int Currency::hash() const {
	int h = 0;
	for (int i = 0; i < strlen(isoCode); i++) {
		h = 26 * h + (isoCode[i] - 26);
	}
	return h;
}

std::array<Currency, 160> allCurrencies = { {
	{"Euro", "EUR", u8"€"},
	{"United States dollar", "USD", u8"$"},
	{"British pound", "GBP", u8"£"},
	{"Swiss franc", "CHF", u8"Fr"},
	{"Cayman Islands dollar", "KYD", u8"$"},
	{"Macedonian denar", "MKD", u8"ден"},
	{"Ethiopian birr", "ETB", u8"Br"},
	{"Zambian kwacha", "ZMW", u8"ZK"},
	{"Georgian lari", "GEL", u8"ლ"},
	{"Mauritian rupee", "MUR", u8"₨"},
	{"Liberian dollar", "LRD", u8"$"},
	{"Chilean peso", "CLP", u8"$"},
	{"Papua New Guinean kina", "PGK", u8"K"},
	{"Moldovan leu", "MDL", u8"L"},
	{"Guernsey pound", "GGP[F]", u8"£"},
	{"Saint Helena pound", "SHP", u8"£"},
	{"East Caribbean dollar", "XCD", u8"$"},
	{"Solomon Islands dollar", "SBD", u8"$"},
	{"Turkmenistan manat", "TMT", u8"m"},
	{"Burmese kyat", "MMK", u8"Ks"},
	{"Armenian dram", "AMD", u8""},
	{"Mauritanian ouguiya", "MRO", u8"UM"},
	{"Paraguayan guaraní", "PYG", u8"₲"},
	{"Belize dollar", "BZD", u8"$"},
	{"South Korean won", "KRW", u8"₩"},
	{"Swedish krona", "SEK", u8"kr"},
	{"Manx pound", "IMP[F]", u8"£"},
	{"CFP franc", "XPF", u8"Fr"},
	{"Tajikistani somoni", "TJS", u8"ЅМ"},
	{"Venezuelan bolívar", "VEF", u8"BsF"},
	{"Yemeni rial", "YER", u8"﷼"},
	{"Nicaraguan córdoba", "NIO", u8"C$"},
	{"Pakistani rupee", "PKR", u8"₨"},
	{"Argentine peso", "ARS", u8"$"},
	{"Central African CFA franc", "XAF", u8"Fr"},
	{"Colombian peso", "COP", u8"$"},
	{"Kuwaiti dinar", "KWD", u8"د.ك"},
	{"Gibraltar pound", "GIP", u8"£"},
	{"United Arab Emirates dirham", "AED", u8"د.إ"},
	{"West African CFA franc", "XOF", u8"Fr"},
	{"New Zealand dollar", "NZD", u8"$"},
	{"Congolese franc", "CDF", u8"Fr"},
	{"Ghana cedi", "GHS", u8"₵"},
	{"Moroccan dirham", "MAD", u8"د.م."},
	{"Libyan dinar", "LYD", u8"ل.د"},
	{"Canadian dollar", "CAD", u8"$"},
	{"Hungarian forint", "HUF", u8"Ft"},
	{"Belarusian ruble", "BYR", u8"Br"},
	{"Icelandic króna", "ISK", u8"kr"},
	{"Bosnia and Herzegovina convertible mark", "BAM", u8"KMorКМ"},
	{"Bhutanese ngultrum", "BTN", u8"Nu."},
	{"Azerbaijani manat", "AZN", u8""},
	{"Mexican peso", "MXN", u8"$"},
	{"Thai baht", "THB", u8"฿"},
	{"Kazakhstani tenge", "KZT", u8""},
	{"Cambodian riel", "KHR", u8"៛"},
	{"Malawian kwacha", "MWK", u8"MK"},
	{"Peruvian nuevo sol", "PEN", u8"S/."},
	{"Norwegian krone", "NOK", u8"kr"},
	{"Brunei dollar", "BND", u8"$"},
	{"Singapore dollar", "SGD", u8"$"},
	{"Hong Kong dollar", "HKD", u8"$"},
	{"Indonesian rupiah", "IDR", u8"Rp"},
	{"Swazi lilangeni", "SZL", u8"L"},
	{"Aruban florin", "AWG", u8"ƒ"},
	{"Rwandan franc", "RWF", u8"Fr"},
	{"Guinean franc", "GNF", u8"Fr"},
	{"Nigerian naira", "NGN", u8"₦"},
	{"Sudanese pound", "SDG", u8"ج.س."},
	{"Vanuatu vatu", "VUV", u8"Vt"},
	{"Serbian dinar", "RSD", u8"дин.ordin."},
	{"Eritrean nakfa", "ERN", u8"Nfk"},
	{"Bangladeshi taka", "BDT", u8"৳"},
	{"Falkland Islands pound", "FKP", u8"£"},
	{"Barbadian dollar", "BBD", u8"$"},
	{"Tunisian dinar", "TND", u8"د.ت"},
	{"New Taiwan dollar", "TWD", u8"$"},
	{"Samoan tālā", "WST", u8"T"},
	{"Kyrgyzstani som", "KGS", u8"лв"},
	{"Philippine peso", "PHP", u8"₱"},
	{"Haitian gourde", "HTG", u8"G"},
	{"Qatari riyal", "QAR", u8"ر.ق"},
	{"Djiboutian franc", "DJF", u8"Fr"},
	{"Comorian franc", "KMF", u8"Fr"},
	{"Australian dollar", "AUD", u8"$"},
	{"Czech koruna", "CZK", u8"Kč"},
	{"Jersey pound", "JEP[F]", u8"£"},
	{"Syrian pound", "SYP", u8"£orل.س"},
	{"Sierra Leonean leone", "SLL", u8"Le"},
	{"Fijian dollar", "FJD", u8"$"},
	{"Japanese yen", "JPY", u8"¥"},
	{"Kenyan shilling", "KES", u8"Sh"},
	{"South African rand", "ZAR", u8"R"},
	{"Angolan kwanza", "AOA", u8"Kz"},
	{"Vietnamese đồng", "VND", u8"₫"},
	{"Saudi riyal", "SAR", u8"ر.س"},
	{"Romanian leu", "RON", u8"lei"},
	{"Namibian dollar", "NAD", u8"$"},
	{"Netherlands Antillean guilder", "ANG", u8"ƒ"},
	{"Macanese pataca", "MOP", u8"P"},
	{"Tongan paʻanga", "TOP", u8"T$"},
	{"North Korean won", "KPW", u8"₩"},
	{"Israeli new shekel", "ILS", u8"₪"},
	{"Botswana pula", "BWP", u8"P"},
	{"Somali shilling", "SOS", u8"Sh"},
	{"Nepalese rupee", "NPR", u8"₨"},
	{"Dominican peso", "DOP", u8"$"},
	{"Mongolian tögrög", "MNT", u8"₮"},
	{"Mozambican metical", "MZN", u8"MT"},
	{"Indian rupee", "INR", u8"₹"},
	{"Tanzanian shilling", "TZS", u8"Sh"},
	{"Cuban peso", "CUP", u8"$"},
	{"Guyanese dollar", "GYD", u8"$"},
	{"Jordanian dinar", "JOD", u8"د.ا"},
	{"Algerian dinar", "DZD", u8"د.ج"},
	{"Lao kip", "LAK", u8"₭"},
	{"Danish krone", "DKK", u8"kr"},
	{"Bermudian dollar", "BMD", u8"$"},
	{"Bahraini dinar", "BHD", u8".د.ب"},
	{"Albanian lek", "ALL", u8"L"},
	{"Polish złoty", "PLN", u8"zł"},
	{"Cape Verdean escudo", "CVE", u8"Escor$"},
	{"Gambian dalasi", "GMD", u8"D"},
	{"Russian ruble", "RUB", u8"RUB"},
	{"Honduran lempira", "HNL", u8"L"},
	{"Bolivian boliviano", "BOB", u8"Bs."},
	{"Uruguayan peso", "UYU", u8"$"},
	{"Transnistrian ruble", "PRB[F]", u8"р."},
	{"Sri Lankan rupee", "LKR", u8"Rsorරු"},
	{"Burundian franc", "BIF", u8"Fr"},
	{"Jamaican dollar", "JMD", u8"$"},
	{"Uzbekistani som", "UZS", u8""},
	{"Guatemalan quetzal", "GTQ", u8"Q"},
	{"Maldivian rufiyaa", "MVR", u8".ރ"},
	{"Croatian kuna", "HRK", u8"kn"},
	{"Chinese yuan", "CNY", u8"¥or元"},
	{"Trinidad and Tobago dollar", "TTD", u8"$"},
	{"Brazilian real", "BRL", u8"R$"},
	{"São Tomé and Príncipe dobra", "STD", u8"Db"},
	{"Lesotho loti", "LSL", u8"L"},
	{"Cuban convertible peso", "CUC", u8"$"},
	{"Lebanese pound", "LBP", u8"ل.ل"},
	{"Panamanian balboa", "PAB", u8"B/."},
	{"Ukrainian hryvnia", "UAH", u8"₴"},
	{"Malaysian ringgit", "MYR", u8"RM"},
	{"Surinamese dollar", "SRD", u8"$"},
	{"Seychellois rupee", "SCR", u8"₨"},
	{"Costa Rican colón", "CRC", u8"₡"},
	{"Omani rial", "OMR", u8"ر.ع."},
	{"Malagasy ariary", "MGA", u8"Ar"},
	{"Bahamian dollar", "BSD", u8"$"},
	{"Bulgarian lev", "BGN", u8"лв"},
	{"Turkish lira", "TRY", u8""},
	{"Iranian rial", "IRR", u8"﷼"},
	{"Ugandan shilling", "UGX", u8"Sh"},
	{"South Sudanese pound", "SSP", u8"£"},
	{"Afghan afghani", "AFN", u8"؋"},
	{"Egyptian pound", "EGP", u8"£orج.م"},
	{"Iraqi dinar", "IQD", u8"ع.د"},
	{"Invalid Currency", "ZZZ", ""}
} };
