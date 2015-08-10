#include "TransferTab.h"
#include "ui_transfertab.h"

#include <QDateTime>
#include "../AccountModel.h"

TransferTab::TransferTab(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TransferTab),
	db(nullptr),
	tagHelper(nullptr, this),
	accountModel(nullptr),
	currentTransferId(-1),
	model(nullptr),
	proxyModel(nullptr),
	stats()
{
	ui->setupUi(this);
}

TransferTab::~TransferTab()
{
	delete ui;
	if (model != nullptr) delete model;
	if (proxyModel != nullptr) delete proxyModel;
}

void TransferTab::init(Db db, AccountModel* accountModel) {
	this->db = db;
	tagHelper.setDb(db);
	this->accountModel = accountModel;

	// model and proxy model for filtering and sorting
	model = new TransferModel(db, this);
	proxyModel = new TransferProxyModel(db, this);
	proxyModel->setSourceModel(model);

	// filter edits
	// TODO: add filter for from own Account
	// connect(ui->trFilterFromAccount, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTransferFilter(int)));
	connect(ui->filterStartDate, SIGNAL(dateTimeChanged(const QDateTime&)), proxyModel, SLOT(setStartDate(const QDateTime&)));
	connect(ui->filterEndDate, SIGNAL(dateTimeChanged(const QDateTime&)), proxyModel, SLOT(setEndDate(const QDateTime&)));
	connect(ui->filterText, SIGNAL(textChanged(const QString&)), proxyModel, SLOT(setFilterText(const QString&)));
	createFilterMonthLinks();

	// stats
	connect(proxyModel, SIGNAL(resetStats()), SLOT(resetStats()));
	connect(proxyModel, SIGNAL(addToStats(int)), SLOT(addToStats(int)));
	connect(proxyModel, SIGNAL(removeFromStats(int)), SLOT(removeFromStats(int)));

	// configure transfer table header
	ui->transfers->setModel(proxyModel);
	ui->transfers->verticalHeader()->hide();
	ui->transfers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->transfers->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // Date
	ui->transfers->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed); // Amount
	ui->transfers->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed); // Checked
	ui->transfers->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed); // Internal
	ui->transfers->horizontalHeader()->setDefaultSectionSize(100);
	ui->transfers->horizontalHeader()->hideSection(1); // From Account

	// enable sorting
	ui->transfers->setSortingEnabled(true);
	ui->transfers->sortByColumn(0, Qt::AscendingOrder);

	// enable row selection
	ui->transfers->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->transfers->setSelectionMode(QAbstractItemView::ExtendedSelection); // support for multi rows and Ctrl/Shift functionality

	ui->transfers->setMouseTracking(true);
	connect(ui->transfers, SIGNAL(entered(const QModelIndex&)), SLOT(setCurrentTransfer(const QModelIndex&)));
	connect(ui->transfers->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), SLOT(showSelected(const QItemSelection&, const QItemSelection&)));

	// auto save changes to tags
	connect(ui->detailsTags, SIGNAL(tagsAdded(QStringList, const std::vector<int>&)), &tagHelper, SLOT(addTransferTags(QStringList, const std::vector<int>&)));
	connect(ui->detailsTags, SIGNAL(tagsRemoved(QStringList, const std::vector<int>&)), &tagHelper, SLOT(removeTransferTags(QStringList, const std::vector<int>&)));

	// details edit
	connect(ui->detailsNote, SIGNAL(textChanged()), SLOT(saveNote()));

	// actions
	connect(ui->checkSelected, SIGNAL(clicked()), SLOT(checkSelected()));
}

void TransferTab::reloadCache() {
	assert_error(model != nullptr);

	model->reloadCache();
}

// TODO: reimplement export transfers
// void TransferTab::exportTransfers() {
// 	auto filename = QFileDialog::getSaveFileName(this, tr("Export Transfers"), QString(), tr("CSV (*.csv)"));
// 	if (filename.isEmpty()) return;
// 	bool hasSelection = ui->transfers->selectionModel()->hasSelection();
// 	auto ids = hasSelection ? tagHelper.transferIds() : stats.includedTransferIds();
// 	model->exportTransfers(filename, ids);
// }

void TransferTab::clickedFilterMonthLink() {
	auto senderButton = (QPushButton*)sender();
	auto text = senderButton->text();

	QDateTime start, end;
	if (text == "all") {
		start = QDateTime::fromMSecsSinceEpoch(0);
		end = QDateTime::fromMSecsSinceEpoch(4398046511104);// 2^42
	} else if (text.length() == 4) {
		// year range
		int year = text.toInt();
		start = QDateTime(QDate(year, 1, 1));
		end = QDateTime(QDate(year + 1, 1, 1)).addSecs(-1);
	} else if (text.length() == 7) {
		// quarter range
		int year = text.left(4).toInt();
		int quarter = text.right(1).toInt();
		start = QDateTime(QDate(year, 3 * quarter - 2, 1));
		end = start.addMonths(3).addSecs(-1);
	} else {
		// month range
		start = QDateTime::fromString(senderButton->text(), "MMM yy");
		if (start.date().year() < 1970) {
			start = start.addYears(100);
		}
		end = start.addMonths(1).addSecs(-1);
	}
	ui->filterStartDate->setDateTime(start);	
	ui->filterEndDate->setDateTime(end);
}

void TransferTab::createFilterMonthLinks() {
	if (model->rowCount() == 0) {
		return;
	}

	db::Transfer tr;
	auto startEnd = (*db)(select(min(tr.date), max(tr.date)).from(tr).where(true));
	assert_error(!startEnd.empty());
	auto start = QDateTime::fromMSecsSinceEpoch(startEnd.front().min).date();
	auto end = QDateTime::fromMSecsSinceEpoch(startEnd.front().max).date();

	auto addButton = [&](const QString& text) {
		assert_error(!text.isEmpty());
		auto button = new QPushButton(text);
		connect(button, SIGNAL(pressed()), SLOT(clickedFilterMonthLink()));
		ui->filterMonthLinks->addWidget(button);
	};

	addButton("all");

	// years
	for (int y = start.year(); y <= end.year(); y++) {
		addButton(QString::number(y));
	}

	// quarters
	while (start <= end) {
		if (start.month() <= 3) {
			addButton(start.toString("yyyy Q1"));
		} else if (start.month() <= 6) {
			addButton(start.toString("yyyy Q2"));
		} else if (start.month() <= 9) {
			addButton(start.toString("yyyy Q3"));
		} else {
			addButton(start.toString("yyyy Q4"));
		}
		start = start.addMonths(3);
	}

	// months (last 3)
	start = end.addMonths(-2);
	while (start <= end) {
		addButton(start.toString("MMM yy"));
		start = start.addMonths(1);
	}

	// last month if needed
	if (start.month() == end.month() && start.day() > end.day()) {
		addButton(start.toString("MMM yy"));
	}
}

void TransferTab::setCurrentTransfer(const QModelIndex& idx) {
	assert_error(model != nullptr);
	assert_error(proxyModel != nullptr);

	const auto& index = proxyModel->mapToSource(idx);
	int id = model->get(index.row()).id;

	// set current id
	if (currentTransferId == id) {
		return;
	}
	currentTransferId = id;

	// clear saved ids of we don't have a selection
	if (!ui->transfers->selectionModel()->hasSelection()) {
		tagHelper.setTransferIds({});
	}

	updateTags();
}

void TransferTab::showSelected(const QItemSelection& selected, const QItemSelection& deseceted) {
	assert_error(model != nullptr);
	assert_error(proxyModel != nullptr);
	assert_error(!selected.isEmpty() || !deseceted.isEmpty());

	for (const auto& idx : deseceted.indexes()) {
		const auto& index = proxyModel->mapToSource(idx);
		if (index.column() != 0) {
			continue;
		}
		int id = model->get(index.row()).id;
		tagHelper.removeTransferId(id);
	}

	for (const auto& idx : selected.indexes()) {
		const auto& index = proxyModel->mapToSource(idx);
		if (index.column() != 0) {
			continue;
		}
		int id = model->get(index.row()).id;
		tagHelper.addTransferId(id);
	}

	updateTags();
}

void TransferTab::checkSelected() {
	assert_debug(!tagHelper.transferIds().empty());
	model->setChecked(tagHelper.transferIds(), true);
}
	
void TransferTab::updateTags() {
if (model->empty()) { return ; } // no transfers yet
	
	assert_error(currentTransferId >= 0);

	const auto& selectedIds = tagHelper.transferIds();
	const auto& ids = selectedIds.empty() ? std::vector<int>(1, currentTransferId) : selectedIds;

	ui->detailsTags->setTags(tagHelper.getTransferTags(ids), ids);

	ui->checkSelected->setVisible(!selectedIds.empty());

	updateDetails();
}

void TransferTab::updateDetails() {
	if (model->empty()) { return ; } // no transfers yet

	int id = tagHelper.transferIds().size() == 1 ? tagHelper.transferIds()[0] : currentTransferId;
	assert_error(id >= 0);
	const auto& transfer = model->getById(id);

	ui->detailsFrom->setText(transfer.from.name);
	ui->detailsReference->setText(transfer.reference);
	ui->detailsNote->setPlainText(transfer.note);
	ui->detailsAccountTags->setText(tagHelper.tagsFromAccounts(transfer.from.id, transfer.to.id).join("; "));
}

void TransferTab::saveNote() {
	if (model->empty()) { return ; } // no transfers yet
	
	const auto& selectedIds = tagHelper.transferIds();
	const auto& id = selectedIds.size() == 1 ? selectedIds[0] : currentTransferId;
	auto note = ui->detailsNote->toPlainText();
	model->setNote(id, note);
}

void TransferTab::updateStats(int transferId, int mode) {
	assert_error(mode >= -1 && mode <= 1);

	// add or remove transfer from stats
	if (mode == 0) {
		// nothing to do, just update ui
	} else {
		assert_error(transferId >= 0);
		const auto& transfer = model->getById(transferId);
		const auto& from = accountModel->getById(transfer.from.id);
		const auto& to = accountModel->getById(transfer.to.id);

		if (mode == -1) {
			// remove
			stats.remove(transfer, transfer.internal || (from.isOwn && to.isOwn));
		} else {
			// add
			stats.add(transfer, transfer.internal || (from.isOwn && to.isOwn));
		}
	}

	// update ui
	ui->statsRevenues->setText(currency(stats.revenues()));
	ui->statsExpenses->setText(currency(stats.expenses()));
	ui->statsInternal->setText(currency(stats.internal()));
	ui->statsProfit->setText(currency(stats.profit()));
}

void TransferTab::resetStats() {
	stats.clear();
	assert_error(stats.revenues() == 0 && stats.expenses() == 0 &&
				 stats.internal() == 0 && stats.profit() == 0, "transfer statistics not cleared correctly");
	updateStats(-1, 0);
}