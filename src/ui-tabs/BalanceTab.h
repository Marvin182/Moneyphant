#ifndef BALANCETAB_H
#define BALANCETAB_H

#include <mr/common>
#include <QWidget>
#include <QColor>
#include <QHash>
#include "../sql.h"
#include "Tab.h"

namespace Ui {
class BalanceTab;
}

class BalanceTab : public Tab
{
	Q_OBJECT
public:
	explicit BalanceTab(QWidget* parent = 0);
	~BalanceTab();

	virtual void init(Db db);

public slots:
	virtual void refresh();
	virtual void focusSearchField() {}
	
private:
	Ui::BalanceTab *ui;

	QHash<int, int> balances;
	QHash<int, QVector<double> > balanceHistory;
	QHash<int, QVector<double> > dateHistory;

	void invalidateCache();
	void replot();
	static QVector<QColor> randomColors(int n, int startHue = -1, int saturation = 200, int value = 240, int alpha = 255);
	static QColor nextRandomColor(const QColor& c);
};

#endif // BALANCETAB_H
