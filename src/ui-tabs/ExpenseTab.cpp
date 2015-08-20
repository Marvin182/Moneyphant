#include "ExpenseTab.h"
#include "ui_ExpenseTab.h"

#include <unordered_set>
#include <QVector>
#include <QDateTime>
#include <mr/timer>
#include "../util.h"

SQLPP_ALIAS_PROVIDER(trTagId);
SQLPP_ALIAS_PROVIDER(accTagId);

constexpr double oneMonth = 30 * 86400;

ExpenseTab::ExpenseTab(QWidget* parent) :
	Tab(parent),
	ui(new Ui::ExpenseTab),
	tagIds(),
	monthsSinceYear0(12 * QDate::currentDate().year() + QDate::currentDate().month() - 1),
	monthlyTagExpenses()
{
	ui->setupUi(this);

	connect(ui->tags, SIGNAL(textEdited(const QString&)), SLOT(onTagsEdited(const QString&)));

	ui->expensePlot->xAxis->setLabel("Month");
	ui->expensePlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
	ui->expensePlot->xAxis->setDateTimeFormat("MMM-yy");
	double now = QDateTime::currentMSecsSinceEpoch() / 1000.0;
	ui->expensePlot->xAxis->setRange(now - 6 * oneMonth, now);

	ui->expensePlot->yAxis->setLabel("Expenses");
	ui->expensePlot->yAxis->grid()->setSubGridVisible(true);
	QPen gridPen(QColor(0, 0, 0, 25));
	gridPen.setStyle(Qt::SolidLine);
	ui->expensePlot->yAxis->grid()->setPen(gridPen);
	gridPen.setStyle(Qt::DotLine);
	ui->expensePlot->yAxis->grid()->setSubGridPen(gridPen);

	ui->expensePlot->legend->setVisible(true);
	ui->expensePlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignHCenter);

	ui->expensePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

ExpenseTab::~ExpenseTab()
{
	delete ui;
}

void ExpenseTab::init(Db db, cqstring lastSearch) {
	Tab::init(db);
	refresh();

	ui->tags->setText(lastSearch);
	onTagsEdited(lastSearch);
}

void ExpenseTab::refresh() {
	recalculateTagExpenses();
	replot();
}

void ExpenseTab::focusSearchField() {
	ui->tags->setFocus();
	ui->tags->selectAll();
}

QString ExpenseTab::searchField() {
	return ui->tags->text();
}

void ExpenseTab::onTagsEdited(cqstring text) {
	db::Tag tg;
	std::vector<int> newTagIds;
	for (cqstring tagName : text.split(' ')) {
		auto t = (*db)(select(tg.id).from(tg).where(tg.name == str(tagName)));
		if (!t.empty()) newTagIds.push_back(t.front().id);
	}
	if (newTagIds != tagIds) {
		tagIds = newTagIds;
		replot();
	}
}

void ExpenseTab::recalculateTagExpenses() {
	monthlyTagExpenses.clear();

	// Spying only a few tags
	// Instead of doing the calculation for all tags, we could just 'spy' some tags and ignore all the rest.
	// This would be faster (~7 ms instead of ~20 ms for all), but we would have to run the calculation every time the tags lists changes.
	// For now I will stick with running the calculation once the tab is shown and just updating the chart and leave the needed code line commented out
	// std::unordered_set<int> spiedTagIds = {22, 23, 25, 33};
	// auto spiedTags = value_list_t<std::unordered_set<int>>(spiedTagIds);
	// assert_warning(spiedTagIds.size() < 16, "QMap is only faster than QHash for less than 16 entries");
	// QMap<int, QVector<double>> monthlyTagExpenses;

	db::Transfer tr;
	db::AccountTag tgA;
	db::TransferTag tgT;

	// due the structure of the following sql statement we might see combinations of transfer and tag multiple times (for statements that have both, own tags and account tags), we will therefore remember the combinations (combined IDs as big number)
	QSet<qint64> seen;

	/* equivalent sql statement:
	 * select tr.id, tr.amount, transferTag.tagId as trTag, accountTag.tagId as accTag from transfer as tr
	 * left join transferTag on tr.id == transferTag.transferId
	 * left join accountTag on (tr.fromId == accountTag.accountId or tr.toId == accountTag.accountId)
	 * order by tr.id
	 */
	for (const auto& t : (*db)(select(tr.id, tr.date, tr.amount, tgT.tagId.as(trTagId), tgA.tagId.as(accTagId)).from(tr
								.left_outer_join(tgT).on(tr.id == tgT.transferId)
								.left_outer_join(tgA).on(tr.fromId == tgA.accountId or tr.toId == tgA.accountId))
								.order_by(tr.date.asc())
								.where(true)
								// .where(tgT.tagId.in(spiedTags) or tgA.tagId.in(spiedTags)) // see comment on spying only a few tags
								)) {
		auto addWithTag = [&](int tagId) {
			// return if we have already seen the transfer tag combination
			qint64 trTagN = 1000000000 * (qint64)t.id + tagId;
			if (seen.contains(trTagN)) return;
			seen += trTagN;

			int monthIdx = relativeMonthIndex(t.date);
			auto& tagExpenses = monthlyTagExpenses[tagId];

			if (tagExpenses.size() < monthIdx + 1) tagExpenses.resize(monthIdx + 1);
			tagExpenses[monthIdx] += t.amount / 100.0;
		};

		if (!t.trTagId.is_null()) addWithTag(t.trTagId);
		if (!t.accTagId.is_null()) addWithTag(t.accTagId);
		// if (!t.trTagId.is_null() && spiedTagIds.find(t.trTagId) != spiedTagIds.end()) addWithTag(t.trTagId); // see comment on spying only a few tags
		// if (!t.accTagId.is_null() && spiedTagIds.find(t.accTagId) != spiedTagIds.end()) addWithTag(t.accTagId);  // see comment on spying only a few tags
	}
}

void ExpenseTab::replot() {
	ui->expensePlot->clearPlottables();

	db::Tag tg;

	// QPen borderPen(QColor::fromHsv(30, 255, 255), 1.5);
	QColor brushColor = QColor::fromHsv(30, 255, 255, 128);
	
	double barWidth = (30 * 86400) / (1.5 * tagIds.size() + 4);

	auto barGroup = new QCPBarsGroup(ui->expensePlot);
	barGroup->setSpacingType(QCPBarsGroup::stPlotCoords);
	barGroup->setSpacing(0.5 * barWidth);

	for (int tagId : tagIds) {
		const auto& ts = (*db)(select(tg.name).from(tg).where(tg.id == tagId));
		auto bar = new QCPBars(ui->expensePlot->xAxis, ui->expensePlot->yAxis);
		ui->expensePlot->addPlottable(bar);
		
		auto borderColor = brushColor.darker();
		borderColor.setAlpha(255);
		bar->setName(qstr(ts.front().name));
		bar->setPen(QPen(borderColor));
		bar->setBrush(brushColor);
		// borderPen.setColor(util::nextBeautifulColor(borderPen.color()));
		brushColor = util::nextBeautifulColor(brushColor);

		bar->setBarsGroup(barGroup);
		bar->setWidthType(QCPBars::wtPlotCoords);
		bar->setWidth(barWidth);

		const auto& x = monthlyTagExpenses[tagId];
		QVector<double> y;
		y.reserve(x.size());
		for (int i = 0; i < x.size(); i++) {
			y += mSecsSinceEpoch(i) / 1000.0;
		}

		bar->setData(y, x);
	}

	ui->expensePlot->yAxis->rescale();

	ui->expensePlot->replot();
}

int ExpenseTab::relativeMonthIndex(qint64 dt) const {
	auto date = QDateTime::fromMSecsSinceEpoch(dt).date();
	int ym = 12 * date.year() + date.month() - 1;
	return monthsSinceYear0 - ym;
}

qint64 ExpenseTab::mSecsSinceEpoch(int relativeIndex) const {
	int ym = monthsSinceYear0 - relativeIndex;
	return QDateTime(QDate(ym / 12, ym % 12 + 1, 15)).toMSecsSinceEpoch();
}

QString ExpenseTab::monthName(int relativeIndex) const {
	int ym = monthsSinceYear0 - relativeIndex;
	return QDate(ym / 12, ym % 12 + 1, 15).toString("MMM-yy");
}

