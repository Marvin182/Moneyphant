#include "BalanceTab.h"
#include "ui_BalanceTab.h"

#include <unordered_set>
#include <QVector>
#include <QDateTime>
#include "../util.h"

constexpr double oneMonth = 30 * 86400;

BalanceTab::BalanceTab(QWidget* parent) :
	Tab(parent),
	ui(new Ui::BalanceTab)
{
	ui->setupUi(this);

	ui->balancePlot->xAxis->setLabel("Date");
	ui->balancePlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
	ui->balancePlot->xAxis->setDateTimeFormat("dd.MM.yy");

	ui->balancePlot->yAxis->setLabel("Balance");

	ui->balancePlot->legend->setVisible(true);
	ui->balancePlot->legend->setSelectableParts(QCPLegend::spItems);

 	auto legend = ui->balancePlot->legend;
	auto selectedFont = legend->font();
	selectedFont.setBold(true);
	legend->setSelectedBorderPen(legend->borderPen());
	legend->setSelectedIconBorderPen(legend->iconBorderPen());
	legend->setSelectedBrush(legend->brush());
	// legend->setSelectedFont(legend->font());
	legend->setSelectedFont(selectedFont);
	legend->setSelectedTextColor(legend->textColor());

	ui->balancePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend | QCP::iMultiSelect);
	ui->balancePlot->setMultiSelectModifier(Qt::NoModifier);


	connect(ui->balancePlot, &QCustomPlot::selectionChangedByUser, [&]() {
	 	auto legend = ui->balancePlot->legend;
		for (int i = 0; i < legend->itemCount(); i++) {
			assert_error(ui->balancePlot->graph(i) != nullptr);
			ui->balancePlot->graph(i)->setVisible(legend->item(i)->selected());
		}
		ui->balancePlot->yAxis->rescale(true);
	});
}

BalanceTab::~BalanceTab()
{
	delete ui;
}

void BalanceTab::init(Db db) {
	Tab::init(db);
	refresh();
}

void BalanceTab::refresh() {
	recalculateHistory();
	replot();
}

void BalanceTab::recalculateHistory() {
	balances.clear();
	balanceHistory.clear();
	dateHistory.clear();

	db::Account acc;
	db::Transfer tr;

	auto startEnd = (*db)(select(min(tr.date), max(tr.date)).from(tr).where(true));
	double startTime = startEnd.front().min;
	double endTime = startEnd.front().max;

	// start with initial balance on first date
	for (auto const& a : (*db)(select(acc.id, acc.initialBalance).from(acc).where(true))) {
		balances[a.id] = a.initialBalance;
		balanceHistory[a.id] += a.initialBalance / 100.0;
		dateHistory[a.id] += startTime / 1000.0;
	}

	// for internal transfer we might have two transfer entries (one from each statement file of the two accounts)
	// therefor we will remember the hash of each internal transfer and exclude the second transfer from the calculation
	// QSet<QString> seenTransfers;
	std::unordered_multiset<std::string> seenTransfersFrom;
	std::unordered_multiset<std::string> seenTransfersTo;
	
	for (const auto& t : (*db)(select(tr.date, tr.fromId, tr.toId, tr.amount, tr.internal).from(tr).order_by(tr.date.asc()).where(true))) {
		bool addFrom = true;
		bool addTo = true;
		if (t.internal) {
			auto it = seenTransfersFrom.find(util::internalTransferHash(t.fromId, - t.amount, t.date));
			if (it != seenTransfersFrom.end()) {
				seenTransfersFrom.erase(it);
				addFrom = false;
			} else {
				seenTransfersTo.insert(util::internalTransferHash(t.fromId, t.amount, t.date));
			}

			it = seenTransfersTo.find(util::internalTransferHash(t.toId, - t.amount, t.date));
			if (it != seenTransfersTo.end()) {
				seenTransfersTo.erase(it);
				addTo = false;
			} else {
				seenTransfersFrom.insert(util::internalTransferHash(t.toId, t.amount, t.date));
			}
		}

		if (addFrom) {
			balances[t.fromId] += t.amount;
			balanceHistory[t.fromId].append((balances[t.fromId] / 100.0));
			dateHistory[t.fromId].append(t.date / 1000.0);
		}
		if (addTo) {
			balances[t.toId] -= t.amount;
			balanceHistory[t.toId].append((balances[t.toId] / 100.0));
			dateHistory[t.toId].append(t.date / 1000.0);
		}
	}

	// end with last balance on end date
	for (auto id : balances.keys()) {
		balanceHistory[id] += balances[id] / 100.0;
		dateHistory[id] += endTime / 1000.0;
	}
}

void BalanceTab::replot() {
	db::Account acc;
	
	ui->balancePlot->clearGraphs();
	QPen pen(QColor::fromHsv(200, 200, 230), 2);
	for (auto const& a : (*db)(select(acc.id, acc.name, acc.balance).from(acc).where(acc.isOwn).order_by(acc.name.asc()))) {
		assert_warning(a.balance == balances[a.id], "calculated balances for account %s do not match, db: %d, cache: %d", cstr(qstr(a.name)), (int)a.balance, balances[a.id]);
		ui->balancePlot->addGraph();
		ui->balancePlot->graph()->setName(qstr(a.name) + " (" + util::formatCurrency(a.balance) + ")");
		ui->balancePlot->graph()->setData(dateHistory[a.id], balanceHistory[a.id]);
		ui->balancePlot->graph()->setLineStyle(QCPGraph::lsLine);
		ui->balancePlot->graph()->setPen(pen);
		pen.setColor(util::nextBeautifulColor(pen.color()));
		ui->balancePlot->legend->item(ui->balancePlot->legend->itemCount() - 1)->setSelected(true);
	}

	double now = QDateTime::currentMSecsSinceEpoch() / 1000.0;
	ui->balancePlot->xAxis->setRange(now - 6 * oneMonth, now);
	ui->balancePlot->yAxis->rescale(true);

	ui->balancePlot->replot();
}

