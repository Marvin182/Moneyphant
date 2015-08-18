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
	invalidateCache();
	replot();
}

void BalanceTab::invalidateCache() {
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
		ui->balancePlot->graph()->setName(qstr(a.name) + " (" + currency(a.balance) + ")");
		ui->balancePlot->graph()->setData(dateHistory[a.id], balanceHistory[a.id]);
		ui->balancePlot->graph()->setLineStyle(QCPGraph::lsLine);
		ui->balancePlot->graph()->setPen(pen);
		pen.setColor(nextRandomColor(pen.color()));
	}

	ui->balancePlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
	ui->balancePlot->xAxis->setDateTimeFormat("dd.MM.yy");

	ui->balancePlot->xAxis->setAutoTickStep(false);
	ui->balancePlot->xAxis->setTickStep(oneMonth);
	ui->balancePlot->xAxis->setSubTickCount(3);

	ui->balancePlot->xAxis->setLabel("Date");
	ui->balancePlot->yAxis->setLabel("Balance");

	ui->balancePlot->yAxis2->setVisible(true);

	ui->balancePlot->rescaleAxes();

	ui->balancePlot->legend->setVisible(true);

	ui->balancePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	ui->balancePlot->replot();
}

QVector<QColor> BalanceTab::randomColors(int n, int startHue, int saturation, int value, int alpha) {
	double hue = startHue == -1 ? mr::random::probability() : startHue / 360.0;
	QVector<QColor> colors;
	colors.reserve(n);
	for (int i = 0; i < n; i++) {
		colors += QColor(360 * hue, saturation, value, alpha);
		hue += mr::constants::goldenRatio - 1;
		if (hue > 1.0) hue--; 
	}
	return colors;
}

QColor BalanceTab::nextRandomColor(const QColor& c) {
	auto h = c.hueF() + mr::constants::goldenRatio - 1;
	if (h > 1.0) h--;
	return QColor::fromHsv(360 * h, c.saturation(), c.value(), c.alpha());
}

