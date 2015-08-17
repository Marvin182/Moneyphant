#include "AccountTab.h"
#include "ui_AccountTab.h"

AccountTab::AccountTab(QWidget* parent) :
	Tab(parent),
	ui(new Ui::AccountTab),
	model(nullptr),
	tagHelper(nullptr, this),
	currentAccountId(-1),
	proxyModel(nullptr)
{
	ui->setupUi(this);
}

AccountTab::~AccountTab()
{
	delete ui;
	if (proxyModel != nullptr) delete proxyModel;
}

void AccountTab::init(Db db, std::shared_ptr<AccountModel> accountModel) {
	Tab::init(db);
	tagHelper.setDb(db);
	this->model = accountModel;
	assert_error(db != nullptr && this->db != nullptr);

	// proxy model for easier filtering and sorting
	proxyModel = new AccountProxyModel(db, this);
	proxyModel->setSourceModel(model.get());

	// filter edits
	connect(ui->filterText, SIGNAL(textChanged(const QString&)), proxyModel, SLOT(setFilterText(const QString&)));

	ui->accounts->setModel(proxyModel);
	ui->accounts->setSortingEnabled(true);
	ui->accounts->sortByColumn(1, Qt::DescendingOrder);

	// enable row selection
	ui->accounts->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->accounts->setSelectionMode(QAbstractItemView::ExtendedSelection); // support for multi rows and Ctrl/Shift functionality

	ui->accounts->setMouseTracking(true);
	connect(ui->accounts, SIGNAL(entered(const QModelIndex&)), SLOT(setCurrentAccount(const QModelIndex&)));
	connect(ui->accounts->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), SLOT(showSelectedAccounts(const QItemSelection&, const QItemSelection&)));
	connect(ui->detailsInitialBalance, SIGNAL(valueChanged(double)), SLOT(setInitialBalance(double)));
	connect(ui->detailsTags, SIGNAL(tagsAdded(QStringList, const std::vector<int>&)), &tagHelper, SLOT(addAccountTags(QStringList, const std::vector<int>&)));
	connect(ui->detailsTags, SIGNAL(tagsRemoved(QStringList, const std::vector<int>&)), &tagHelper, SLOT(removeAccountTags(QStringList, const std::vector<int>&)));
	connect(ui->mergeAccounts, SIGNAL(clicked()), SLOT(mergeAccounts()));

	ui->mergeAccounts->setVisible(false);
	connect(this, &AccountTab::mergeAccountsEnabled, [&](bool enabled) { ui->mergeAccounts->setVisible(enabled); });

	// configure headers
	ui->accounts->verticalHeader()->hide();
	ui->accounts->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->accounts->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
	ui->accounts->horizontalHeader()->resizeSection(1, 60);
	ui->accounts->horizontalHeader()->setSectionResizeMode(9, QHeaderView::Fixed);
	ui->accounts->horizontalHeader()->resizeSection(9, 120);
	ui->accounts->horizontalHeader()->hideSection(0); // id
	ui->accounts->horizontalHeader()->hideSection(6); // account number
	ui->accounts->horizontalHeader()->hideSection(7); // bank code
	ui->accounts->horizontalHeader()->hideSection(8); // initial balance
	ui->accounts->horizontalHeader()->hideSection(10); // tags

	ui->accounts->horizontalHeader()->moveSection(1, 0);
	ui->accounts->horizontalHeader()->moveSection(9, 1);
}

void AccountTab::focusSearchField() {
	ui->filterText->setFocus();
}

void AccountTab::setCurrentAccount(const QModelIndex& idx) {
	const auto& index = proxyModel->mapToSource(idx);
	int id = model->get(index.row()).id;
	if (currentAccountId == id) {
		return;
	}
	currentAccountId = id;

	auto selectionModel = ui->accounts->selectionModel();
	if (!selectionModel->hasSelection()) {
		tagHelper.setAccountIds({currentAccountId});
	}

	updateAccountInfo();
}

void AccountTab::showSelectedAccounts(const QItemSelection& selected, const QItemSelection& deseceted) {
	assert_error(!selected.isEmpty() || !deseceted.isEmpty());

	for (const auto& idx : deseceted.indexes()) {
		const auto& index = proxyModel->mapToSource(idx);
		if (index.column() != 0) {
			continue;
		}
		int id = model->get(index.row()).id;
		tagHelper.removeAccountId(id);
	}

	for (const auto& idx : selected.indexes()) {
		const auto& index = proxyModel->mapToSource(idx);
		if (index.column() != 0) {
			continue;
		}
		int id = model->get(index.row()).id;
		tagHelper.addAccountId(id);
	}

	updateAccountInfo();
}
	
void AccountTab::updateAccountInfo() {
	assert_error(currentAccountId >= 0);

	const auto& ids = tagHelper.accountIds();
	auto selectionModel = ui->accounts->selectionModel();

	ui->detailsTags->setEnabled(selectionModel->hasSelection());
	ui->moreAccountTags->setText("");
	emit mergeAccountsEnabled(ids.size() == 2);

	if (ids.empty()) {
		return;
	} else if (ids.size() == 1) {
		ui->detailsTags->setTags(tagHelper.getAccountTags(), tagHelper.accountIds());
	} else {
		const auto& commonTags = tagHelper.getAccountTags();
		ui->detailsTags->setTags(commonTags, tagHelper.accountIds());

		const auto& currentAccountTags = tagHelper.getAccountTags({currentAccountId});
		QStringList moreTags;
		for (const auto& t : currentAccountTags) {
			if (!commonTags.contains(t)) {
				moreTags << t;
			}
		}
		ui->moreAccountTags->setText(moreTags.join(" "));
	}

	updateAccountDetails();
}

void AccountTab::updateAccountDetails() {
	const auto& selectedIds = tagHelper.accountIds();
	bool editEnabled = selectedIds.size() == 1;

	ui->detailsInitialBalance->setEnabled(editEnabled);

	const auto& account = model->getById(editEnabled ? selectedIds[0] : currentAccountId);

	ui->detailsName->setText(account.name);
	ui->detailsOwner->setText(account.owner);
	ui->detailsIban->setText(account.iban);
	ui->detailsBic->setText(account.bic);
	ui->detailsAccountNumber->setText(account.accountNumber);
	ui->detailsBankCode->setText(account.bankCode);
	ui->detailsInitialBalance->setValue(account.initialBalance / 100.0);
}

void AccountTab::setInitialBalance(double d) {
	if (tagHelper.accountIds().size() != 1) return;
	int val = d * 100;
	model->setInitialBalance(tagHelper.accountIds()[0], 100 * d);
}

void AccountTab::mergeAccounts() {
	assert_error(tagHelper.accountIds().size() == 2);
	model->mergeAccounts(tagHelper.accountIds().front(), tagHelper.accountIds().back());
	ui->accounts->selectionModel()->clearSelection();
	tagHelper.setAccountIds({});
}
