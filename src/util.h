#ifndef MONEYPHANT_UTIL_H
#define MONEYPHANT_UTIL_H

#include <string>
#include <QtGlobal>

namespace util {

std::string internalTransferHash(int accountId, int amount, qint64 dt);

} // namespace util

#endif // MONEYPHANT_UTIL_H
