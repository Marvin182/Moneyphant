#ifndef MONEYPHANT_UTIL_H
#define MONEYPHANT_UTIL_H

#include <mr/common>
#include <QtGlobal>
#include <QVector>
#include <QColor>

namespace util {

/** Creates a string with the amount and a date range that should be similar for transfers that have the same amount but are a few days apart.
 * The 'hash' string will contain the amount, year and month for dt and year and month for the previous/next month depending on whether dt is
 * in the first or second half if the month.
 */
std::string internalTransferHash(int accountId, int amount, qint64 dt);

/** Very robust parsing method for currencies with 1 or 2 decimals.
 * parseCurrency will accept different combinations of thousand (comma/period) and decimal separators (comma/period).
 * It does so by trying to find the decimal separator (= the one that only follow by 1 or 2 digits) and ignoring all other separators.
 */
int parseCurrency(cqstring s);

/** Just a simple formating using the default QLocal */
inline QString formatCurrency(int amount, cqstring symbol = "€") { return QLocale().toCurrencyString(amount / 100.0, symbol); }

/** Uses the golden ration to return a color with a different hue value.
 * Repeatedly calling the method will often result in a stream of nice looking colors that are optical easy to distinguish.
 */
QColor nextBeautifulColor(const QColor& c);

/** Returns n random colors in the same manner as nextBeautifulColor(). */
QVector<QColor> randomColors(int n, int saturation = 200, int value = 240, int alpha = 255);

} // namespace util

#endif // MONEYPHANT_UTIL_H
