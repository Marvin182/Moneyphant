#ifndef MONEYPHANT_UTIL_H
#define MONEYPHANT_UTIL_H

#include <mr/common>
#include <QtGlobal>

namespace util {

std::string internalTransferHash(int accountId, int amount, qint64 dt);

int parseCurrency(cqstring s);

} // namespace util

#endif // MONEYPHANT_UTIL_H
