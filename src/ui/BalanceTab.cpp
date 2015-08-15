#include "BalanceTab.h"
#include "ui_BalanceTab.h"

#include <QSet>
#include <QHash>
#include <QVector>
#include <QDateTime>

BalanceTab::BalanceTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BalanceTab)
{
	ui->setupUi(this);
}

BalanceTab::~BalanceTab()
{
	delete ui;
}

void BalanceTab::init(Db db) {
	this->db = db;
	reloadCache();
}

void BalanceTab::reloadCache() {
	qLog() << "graph count: " << ui->balancePlot->graphCount();

	ui->balancePlot->clearGraphs();

	db::Account acc;
	db::Transfer tr;

	QSet<int> ownAccountIds;
	QHash<int, QVector<double> > balances;
	QHash<int, QVector<double> > times;

	auto startEnd = (*db)(select(min(tr.date), max(tr.date)).from(tr).where(true));
	double startTime = startEnd.front().min / 1000.0;
	double endTime = startEnd.front().max / 1000.0;

	for (auto const& a : (*db)(select(acc.id, acc.initialBalance).from(acc).where(acc.isOwn))) {
		ownAccountIds += a.id;
		balances[a.id] += a.initialBalance / 100.0;
		times[a.id] += startTime;
	}
	for (auto const& t : (*db)(select(tr.fromId, tr.toId, tr.date, tr.amount, tr.internal).from(tr).order_by(tr.date.asc()).where(true))) {
		if (ownAccountIds.contains(t.fromId)) {
			balances[t.fromId] += (balances[t.fromId].last() - t.amount / 100.0);
			times[t.fromId] += t.date / 1000.0;
			// qLog() << times[t.fromId].last() << "|" << balances[t.fromId].last();
		}
		// internal?
		if (ownAccountIds.contains(t.toId)) {
			balances[t.toId] += (balances[t.toId].last() + t.amount / 100.0);
			times[t.toId] += t.date / 1000.0;
		}
	}
	QPen pen(QColor::fromHsv(mr::random::probability() * 360, 200, 240), 2);
	for (auto const& a : (*db)(select(acc.id, acc.name).from(acc).where(acc.isOwn).order_by(acc.name.asc()))) {
		assert_error(ownAccountIds.contains(a.id));
		balances[a.id] += balances[a.id].last();
		times[a.id] += endTime;

		ui->balancePlot->addGraph();
		ui->balancePlot->graph()->setName(qstr(a.name));
		ui->balancePlot->graph()->setData(times[a.id], balances[a.id]);
		ui->balancePlot->graph()->setLineStyle(QCPGraph::lsLine);
		ui->balancePlot->graph()->setPen(pen);
		pen.setColor(nextRandomColor(pen.color()));

	}

	ui->balancePlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
	ui->balancePlot->xAxis->setDateTimeFormat("dd.MM.yy");

	ui->balancePlot->xAxis->setAutoTickStep(false);
	ui->balancePlot->xAxis->setTickStep(2628000); // one month in seconds
	ui->balancePlot->xAxis->setSubTickCount(3);

	ui->balancePlot->xAxis->setLabel("Date");
	ui->balancePlot->yAxis->setLabel("Balance");

	// ui->balancePlot->yAxis2->setVisible(true);

	ui->balancePlot->xAxis->setRange(startTime, endTime);
	// ui->balancePlot->rescaleAxes();
	ui->balancePlot->yAxis->rescale();

	ui->balancePlot->legend->setVisible(true);

	ui->balancePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	ui->balancePlot->replot();

	qLog() << "graph count: " << ui->balancePlot->graphCount();
}

QVector<QColor> BalanceTab::randomColors(int n, int startHue, int saturation, int value, int alpha) {
	double hue = startHue == -1 ? mr::random::probability() : startHue / 360.0;
	QVector<QColor> colors;
	colors.reserve(n);
	for (int i = 0; i < n; i++) {
		colors += QColor(360 * hue, saturation, value, alpha);
		hue += 0.618033988749895;
		if (hue > 1.0) hue--; 
	}
	return colors;
}

QColor BalanceTab::nextRandomColor(const QColor& c) {
	auto h = c.hueF() + 0.618033988749895;
	if (h > 1.0) h--;
	return QColor::fromHsv(360 * h, c.saturation(), c.value(), c.alpha());
}

