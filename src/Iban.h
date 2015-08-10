#ifndef IBAN_H
#define IBAN_H

#include <array>
#include <string.h>
#include <mr/common>

// DRAFT for an advanced IBAN class and exact formating

struct IbanFormat {
	int len;
	const char* countryName;
	const char* bbanFormat;
	const char* ibanFields;
	const char* fixedCheckDigits;

	constexpr IbanFormat(const char* countryName, int len, const char* bbanFormat, const char* ibanFields, const char* fixedCheckDigits = nullptr) :
		len(len), countryName(countryName), bbanFormat(bbanFormat), ibanFields(ibanFields), fixedCheckDigits(fixedCheckDigits)
	{}

	QString countryCode() const { return QString::fromUtf8(ibanFields, 2); }
	bool hasFixedCheckDigit() const { return fixedCheckDigits != nullptr && strlen(fixedCheckDigits) > 0; }
};

/* Explanations for IBAN fields:
 * b = National bank code
 * s = Branch code
 * x = National check digit
 * c = Account number
 * k = IBAN check digits
 * n = Owner account number ("1", "2" etc.) (http://www.bcb.gov.br/Pom/Spb/Ing/IBAN-Guidelines_English.pdf)
 * t = Account type (Cheque account, Savings account etc.)
 * g = Branch code (fr:code guichet)
 * m = Currency
 * i = holder's kennitala (national identification number)
 * 0 = Zeroes
 * d = Currency Symbol
 */
constexpr std::array<IbanFormat, 68> InternationalIbanFormats = {{
	{"Albania", 28, "8n,16c", "ALkk bbbs sssx cccc cccc cccc cccc"},
	{"Andorra", 24, "8n,12c", "ADkk bbbb ssss cccc cccc cccc"},
	{"Austria", 20, "16n", "ATkk bbbb bccc cccc cccc"},
	{"Azerbaijan", 28, "4c,20n", "AZkk bbbb cccc cccc cccc cccc cccc"},
	{"Bahrain", 22, "4a,14c", "BHkk bbbb cccc cccc cccc cc"},
	{"Belgium", 16, "12n", "BEkk bbbc cccc ccxx"},
	{"Bosnia and Herzegovina", 20, "16n", "BAkk bbbs sscc cccc ccxx", "39"},
	{"Brazil", 29, "23n, 1a, 1c", "BRkk bbbb bbbb ssss sccc cccc ccct n"}, // k = IBAN check digits (Calculated by MOD 97-10)
	{"Bulgaria", 22, "4a,6n,8c", "BGkk bbbb ssss ddcc cccc cc"},
	{"Costa Rica", 21, "17n", "CRkk bbbc cccc cccc cccc c"},
	{"Croatia", 21, "17n", "HRkk bbbb bbbc cccc cccc c"},
	{"Cyprus", 28, "8n,16c", "CYkk bbbs ssss cccc cccc cccc cccc"},
	{"Czech Republic", 24, "20n", "CZkk bbbb ssss sscc cccc cccc"},
	{"Denmark", 18, "14n", "DKkk bbbb cccc cccc cc"},
	{"Dominican Republic", 28, "4a,20n", "DOkk bbbb cccc cccc cccc cccc cccc"},
	{"East Timor", 23, "19n", "TLkk bbbc cccc cccc cccc cxx", "38"},
	{"Estonia", 20, "16n", "EEkk bbss cccc cccc cccx"},
	{"Faroe Islands[Note 4]", 18, "14n", "FOkk bbbb cccc cccc cx"},
	{"Finland", 18, "14n", "FIkk bbbb bbcc cccc cx"},
	{"France[Note 5]", 27, "10n,11c,2n", "FRkk bbbb bggg ggcc cccc cccc cxx"},
	{"Georgia", 22, "2c,16n", "GEkk bbcc cccc cccc cccc cc"},
	{"Germany", 22, "18n", "DEkk bbbb bbbb cccc cccc cc"},
	{"Gibraltar", 23, "4a,15c", "GIkk bbbb cccc cccc cccc ccc"},
	{"Greece", 27, "7n,16c", "GRkk bbbs sssc cccc cccc cccc ccc"},
	{"Greenland[Note 4]", 18, "14n", "GLkk bbbb cccc cccc cc"},
	{"Guatemala [32]", 28, "4c,20c", "GTkk bbbb mmtt cccc cccc cccc cccc"},
	{"Hungary", 28, "24n", "HUkk bbbs sssk cccc cccc cccc cccx"},
	{"Iceland", 26, "22n", "ISkk bbbb sscc cccc iiii iiii ii"},
	{"Ireland", 22, "4c,14n", "IEkk aaaa bbbb bbcc cccc cc"},
	{"Israel", 23, "19n", "ILkk bbbn nncc cccc cccc ccc"},
	{"Italy", 27, "1a,10n,12c", "ITkk xaaa aabb bbbc cccc cccc ccc"},
	{"Jordan[33]", 30, "4a, 22n", "JOkk bbbb nnnn cccc cccc cccc cccc cc"},
	{"Kazakhstan", 20, "3n,13c", "KZkk bbbc cccc cccc cccc"},
	{"Kosovo", 20, "4n,10n,2n", "XKkk bbbb cccc cccc cccc"},
	{"Kuwait", 30, "4a, 22c", "KWkk bbbb cccc cccc cccc cccc cccc cc"},
	{"Latvia", 21, "4a,13c", "LVkk bbbb cccc cccc cccc c"},
	{"Lebanon", 28, "4n,20c", "LBkk bbbb cccc cccc cccc cccc cccc"},
	{"Liechtenstein", 21, "5n,12c", "LIkk bbbb bccc cccc cccc c"},
	{"Lithuania", 20, "16n", "LTkk bbbb bccc cccc cccc"},
	{"Luxembourg", 20, "3n,13c", "LUkk bbbc cccc cccc cccc"},
	{"Macedonia", 19, "3n,10c,2n", "MKkk bbbc cccc cccc cxx", "07"},
	{"Malta", 31, "4a,5n,18c", "MTkk bbbb ssss sccc cccc cccc cccc ccc"},
	{"Mauritania", 27, "23n", "MRkk bbbb bsss sscc cccc cccc cxx"},
	{"Mauritius", 30, "4a,19n,3a", "MUkk bbbb bbss cccc cccc cccc 000d dd"},
	{"Monaco", 27, "10n,11c,2n", "MCkk bbbb bsss sscc cccc cccc cxx"},
	{"Moldova", 24, "2c,18c", "MDkk bbcc cccc cccc cccc cccc"},
	{"Montenegro", 22, "18n", "MEkk bbbc cccc cccc cccc xx", "25"},
	{"Netherlands[Note 6]", 18, "4a,10n", "NLkk bbbb cccc cccc cc"},
	{"Norway", 15, "11n", "NOkk bbbb cccc ccx"}, // x = Modulo-11 national check digit
	{"Pakistan", 24, "4c,16n", "PKkk bbbb cccc cccc cccc cccc"},
	{"Palestinian", 29, "4c,21n", "PSkk bbbb xxxx xxxx xccc cccc cccc c"}, // x = Not specified
	{"Poland", 28, "24n", "PLkk bbbs sssx cccc cccc cccc cccc"},
	{"Portugal", 25, "21n", "PTkk bbbb ssss cccc cccc cccx x", "50"},
	{"Qatar", 29, "4a, 21c", "QAkk bbbb cccc cccc cccc cccc cccc c"}, // c = Account number (http://www.swift.com/dsp/resources/documents/IBAN_Registry.pdf)
	{"Romania", 24, "4a,16c", "ROkk bbbb cccc cccc cccc cccc"},
	{"San Marino", 27, "1a,10n,12c", "SMkk xaaa aabb bbbc cccc cccc ccc"},
	{"Saudi Arabia", 24, "2n,18c", "SAkk bbcc cccc cccc cccc cccc"},
	{"Serbia", 22, "18n", "RSkk bbbc cccc cccc cccc xx"},
	{"Slovakia", 24, "20n", "SKkk bbbb ssss sscc cccc cccc"},
	{"Slovenia", 19, "15n", "SIkk bbss sccc cccc cxx"},
	{"Spain", 24, "20n", "ESkk bbbb gggg xxcc cccc cccc"},
	{"Sweden", 24, "20n", "SEkk bbbc cccc cccc cccc cccc"},
	{"Switzerland", 21, "5n,12c", "CHkk bbbb bccc cccc cccc c"},
	{"Tunisia", 24, "20n", "TNkk bbss sccc cccc cccc cccc", "59"},
	{"Turkey", 26, "5n,17c", "TRkk bbbb bxcc cccc cccc cccc cc"}, // x = Reserved for future use (currently "0")
	{"United Arab Emirates", 23, "3n,16n", "AEkk bbbc cccc cccc cccc ccc"},
	{"United Kingdom[Note 7]", 22, "4a,14n", "GBkk bbbb ssss sscc cccc cc"},
	{"Virgin Islands, British", 24, "4c,16n", "VGkk bbbb cccc cccc cccc cccc"}
}};

#endif // IBAN_H