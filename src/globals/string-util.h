#ifndef GLOBALS_STRING_UTIL_H
#define GLOBALS_STRING_UTIL_H

#include <QString>

using cqstring = const QString&;

const char* cstr(cqstring s);
std::string str(cqstring s);
QString qstr(const std::string& s);

std::ostream& operator<<(std::ostream& os, cqstring s);

QString currency(int amount, cqstring symbol = "€");

bool fuzzyMatch(cqstring text, cqstring needle);

#endif // GLOBALS_STRING_UTIL_H
