#include "ExpenseTab.h"
#include "ui_ExpenseTab.h"

#include <unordered_set>
#include <chrono>
#include <QVector>
#include <QDateTime>
#include <date.h>
#include <mr/timer>
#include "../util.h"

SQLPP_ALIAS_PROVIDER(trTagId);
SQLPP_ALIAS_PROVIDER(accTagId);

ExpenseTab::ExpenseTab(QWidget* parent) :
	Tab(parent),
	ui(new Ui::ExpenseTab),
	tagIds(),
	monthsSinceYear0(12 * QDate::currentDate().year() + QDate::currentDate().month() - 1),
	historyLen(0),
	monthlyTagExpenses()
{
	ui->setupUi(this);

	connect(ui->tags, SIGNAL(textEdited(const QString&)), SLOT(onTagsEdited(const QString&)));

	ui->expensePlot->xAxis->setLabel("Month");
	ui->expensePlot->xAxis->setAutoTicks(false);
	ui->expensePlot->xAxis->setAutoTickLabels(false);
	ui->expensePlot->xAxis->setTickLabelRotation(60);
	ui->expensePlot->xAxis->setSubTickCount(0);
	ui->expensePlot->xAxis->setRange(0, 6);
	ui->expensePlot->xAxis->setRangeReversed(true);

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

template <class Duration>
constexpr std::chrono::seconds::rep secondsSinceEpoche(const std::chrono::time_point<std::chrono::system_clock, Duration>& dp) {
	return std::chrono::duration_cast<std::chrono::seconds>(dp.time_since_epoch()).count();
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
	auto now = std::chrono::system_clock::now();
	for (const auto& t : (*db)(select(tr.id, tr.ymd, tr.amount, tgT.tagId.as(trTagId), tgA.tagId.as(accTagId)).from(tr
								.left_outer_join(tgT).on(tr.id == tgT.transferId)
								.left_outer_join(tgA).on(tr.fromId == tgA.accountId or tr.toId == tgA.accountId))
								.order_by(tr.ymd.asc())
								.where(true)
								// .where(tgT.tagId.in(spiedTags) or tgA.tagId.in(spiedTags)) // see comment on spying only a few tags
								)) {
		auto addWithTag = [&](int tagId) {
			// return if we have already seen the transfer tag combination
			qint64 trTagN = 1000000000 * (qint64)t.id + tagId;
			if (seen.contains(trTagN)) return;
			seen += trTagN;

			int monthIdx = std::chrono::duration_cast<date::months>(now - t.ymd.value()).count();
			auto& tagExpenses = monthlyTagExpenses[tagId];

			historyLen = std::max(historyLen, monthIdx + 1);
			if (tagExpenses.size() < historyLen) tagExpenses.resize(historyLen);
			tagExpenses[monthIdx] += t.amount / 100.0;
		};

		if (!t.trTagId.is_null()) addWithTag(t.trTagId);
		if (!t.accTagId.is_null()) addWithTag(t.accTagId);
		// if (!t.trTagId.is_null() && spiedTagIds.find(t.trTagId) != spiedTagIds.end()) addWithTag(t.trTagId); // see comment on spying only a few tags
		// if (!t.accTagId.is_null() && spiedTagIds.find(t.accTagId) != spiedTagIds.end()) addWithTag(t.accTagId);  // see comment on spying only a few tags
	}

	for (auto it = monthlyTagExpenses.begin(); it != monthlyTagExpenses.end(); it++) {
		assert_error(it.value().size() <= historyLen);
		it.value().resize(historyLen);
	}
}

void ExpenseTab::replot() {
	ui->expensePlot->clearPlottables();

	db::Tag tg;

	// monthly ticks and individual labels
	QVector<double> ticks(historyLen);
	QVector<QString> tickLabels(historyLen);
	date::year_month_day ymd = date::floor<date::days>(std::chrono::system_clock::now());
	for (int i = 0; i < historyLen; i++) {
		ticks[i] = i;
		tickLabels[i] = util::toQDate(ymd - date::months{i}).toString("MMM-yy");
	}
	ui->expensePlot->xAxis->setTickVector(ticks);
	ui->expensePlot->xAxis->setTickVectorLabels(tickLabels);

	QColor brushColor = QColor::fromHsv(30, 255, 255, 128);
	auto barGroup = new QCPBarsGroup(ui->expensePlot);
	for (int tagId : tagIds) {
		const auto& ts = (*db)(select(tg.name).from(tg).where(tg.id == tagId));
		auto bar = new QCPBars(ui->expensePlot->xAxis, ui->expensePlot->yAxis);
		ui->expensePlot->addPlottable(bar);
		
		auto borderColor = brushColor.darker();
		borderColor.setAlpha(255);
		bar->setName(qstr(ts.front().name));
		bar->setPen(QPen(borderColor));
		bar->setBrush(brushColor);
		brushColor = util::nextBeautifulColor(brushColor);

		bar->setBarsGroup(barGroup);
		bar->setWidth(0.5 / tagIds.size());

		bar->setData(ticks, monthlyTagExpenses[tagId]);
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

