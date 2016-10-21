#ifndef EXPENSETAB_H
#define EXPENSETAB_H

#include <vector>
#include <QHash>
#include <mr/common>
#include <mr/qt>
#include "Tab.h"

namespace Ui {
class ExpenseTab;
}

class ExpenseTab : public Tab
{
	Q_OBJECT
public:
	explicit ExpenseTab(QWidget* parent = 0);
	~ExpenseTab();

	using Tab::init;
	virtual void init(Db db, cqstring lastSearch);
	QString searchField();

public slots:
	virtual void refresh();
	virtual void focusSearchField();
	
private:
	Ui::ExpenseTab *ui;

	std::vector<int> tagIds;
	int monthsSinceYear0;
	int historyLen;
	QHash<int, QVector<double>> monthlyTagExpenses;

private slots:
	void onTagsEdited(const QString& text);
	void recalculateTagExpenses();
	void replot();

private:
	int relativeMonthIndex(qint64 dt) const;
	qint64 mSecsSinceEpoch(int relativeIndex) const;
	QString monthName(int relativeIndex) const;
};

#endif // EXPENSETAB_H
