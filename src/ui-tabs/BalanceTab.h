#ifndef BALANCETAB_H
#define BALANCETAB_H

#include <mr/common>
#include <mr/qt>
#include <QHash>
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

	void recalculateHistory();
	void replot();
};

#endif // BALANCETAB_H
