#include "AccountModel.h"

#include <QFile>
#include <mr/common>
#include "../util.h"

constexpr int COLUMNS_COUNT = 11;

AccountModel::AccountModel(Db db, QObject* parent) :
	QAbstractTableModel(parent),
	db(db),
	cachedAccounts(0)
{
	invalidateCache();
}

int AccountModel::rowCount(const QModelIndex& parent) const {
	assert_debug(parent == QModelIndex());
	return cachedAccounts.size();
}

int AccountModel::columnCount(const QModelIndex& parent) const {
	assert_debug(parent == QModelIndex());
	return COLUMNS_COUNT;
}

QVariant AccountModel::data(const QModelIndex& index, int role) const {
	assertValidIndex(index);

	const auto& a = get(index.row());

	switch (role) {
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
		case Qt::EditRole:
			switch (index.column()) {
				case 0: return a.id;
				case 1: return "";
				case 2: return a.name;
				case 3: return a.owner;
				case 4: return a.iban.isEmpty() ? a.accountNumber : a.formattedIban();
				case 5: return a.bic.isEmpty() ? mr::separateGroups(a.bankCode, 3, ' ') : a.bic;
				case 6: return a.accountNumber;
				case 7: return mr::separateGroups(a.bankCode, 3, ' ');
				case 8: return util::formatCurrency(a.initialBalance);
				case 9: return util::formatCurrency(a.balance);
				case 10: return ""; // tags
				default: assert_unreachable();
			}
			break;
		case Qt::CheckStateRole:
			switch (index.column()) {
				case 1:	return a.isOwn ? Qt::Checked : Qt::Unchecked;
			}
			break;
		case Qt::UserRole + 1:
			return QString(a.name + a.owner + a.iban);
			break;
	}

	return QVariant();
}

QVariant AccountModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case 0: return tr("ID");
			case 1: return tr("Own");
			case 2: return tr("Name");
			case 3: return tr("Owner");
			case 4: return tr("IBAN / Account Number");
			case 5: return tr("BIC / Bank Code");
			case 6: return tr("Account Number");
			case 7: return tr("Bank Code");
			case 8: return tr("Initial Balance");
			case 9: return tr("Balance");
			case 10: return tr("Tags");
			default: assert_unreachable();
		}
	}
	
	return QVariant();
}

bool AccountModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	assertValidIndex(index);
	auto& a = _get(index.row());
	
	db::Account acc;
	if (index.column() == 1) {
		assert_error(value.canConvert<bool>());
		assert_error(value.toBool() != a.isOwn);
		a.isOwn = value.toBool();
		(*db)(update(acc).set(acc.isOwn = a.isOwn).where(acc.id == a.id));
	} else {
		assert_error(value.canConvert<QString>());
		switch (index.column()) {
			case 2:
				a.name = value.toString();
				break;
			case 3:
				a.owner = value.toString();
				break;
			case 4:
				a.setIban(value.toString());
				break;
			case 5:
				a.setBic(value.toString());
			default:
				assert_error(false);
				return false;
		}

		(*db)(update(acc).set(acc.isOwn = a.isOwn,
								acc.name = str(a.name),
								acc.owner = str(a.owner),
								acc.iban = str(a.iban),
								acc.bic = str(a.bic),
								acc.accountNumber = str(a.accountNumber),
								acc.bankCode = str(a.bankCode),
								acc.initialBalance = a.initialBalance
								).where(acc.id == a.id));
	}

	emit dataChanged(index, index);
	return true;
}

Qt::ItemFlags AccountModel::flags(const QModelIndex& index) const {
	assertValidIndex(index);

	// special account
	if (index.row() == 0) return Qt::NoItemFlags;

	auto commonFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	switch (index.column()) {
		case 1:
			return commonFlags | Qt::ItemIsUserCheckable;
		case 2:
		case 3:
		case 4:
		case 5:
			return commonFlags | Qt::ItemIsEditable;
		default:
			return commonFlags;
	}
}

const Account& AccountModel::get(int row) const {
	assert_error(row >= 0 && row < cachedAccounts.size());
	return cachedAccounts[row];
}

const Account& AccountModel::getById(int id) const {
	assert_error(id >= 0 && id2Row.find(id) != id2Row.end());
	int row = id2Row.at(id);
	return get(row);
}

QSet<int> AccountModel::ownAccountIds(Db db) {
	db::Account acc;
	QSet<int> ids;
	for (const auto& a : (*db)(select(acc.id).from(acc).where(acc.isOwn))) {
		ids += a.id;
	}
	return ids;
}


void AccountModel::invalidateCache() {
	emit beginResetModel();

	db::Account acc;
	id2Row.clear();
	cachedAccounts.clear();

	int rowCount = (*db)(select(count(acc.id)).from(acc).where(true)).front().count;
	cachedAccounts.reserve(rowCount + 1);
	id2Row.reserve(rowCount + 1);
	
	// add special "Own Accounts" account
	cachedAccounts.push_back(Account(0, true, 0, "Own Accounts", "", "", "", "", "", 0));
	id2Row[0] = 0;

	int row = 1;
	for (const auto& a : (*db)(select(all_of(acc)).from(acc).where(true).order_by(acc.isOwn.desc(), acc.name.asc()))) {
		id2Row[a.id] = row++;
		cachedAccounts.push_back({(int)a.id, a.isOwn, (int)a.balance, qstr(a.name), qstr(a.owner), qstr(a.iban), qstr(a.bic), qstr(a.accountNumber), qstr(a.bankCode), (int)a.initialBalance});
		if (a.isOwn) {
			cachedAccounts.front().balance += a.balance;
			cachedAccounts.front().initialBalance += a.initialBalance;
		}
	}

	emit endResetModel();
}

void AccountModel::createBackup(const QString& path) {
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		assert_error(false && "Could not open accounts backup file");
	}
	for (const auto& a : cachedAccounts) {
		file.write(QString("%1;%2;%3;%4;%5;%6;%7;%8\n").arg(a.id).arg(a.isOwn ? '1' : '0').arg(a.name).arg(a.owner).arg(a.iban).arg(a.bic).arg(a.accountNumber).arg(a.bankCode).toLocal8Bit());
	}
}

void AccountModel::setInitialBalance(int id, int value) {
	auto& account = _getById(id);
	if (account.initialBalance == value) return;
	account.balance += value - account.initialBalance;
	account.initialBalance = value;
	save(account);

	emitChanged(account.id);
}

void AccountModel::mergeAccounts(int firstId, int secondId) {
	auto& first = _getById(firstId);
	auto& second = _getById(secondId);

	db::Account acc;
	db::Transfer tr;
	db::AccountTag accTag;

	// update first account with merged information
	first.isOwn = first.isOwn | second.isOwn;
	if (first.name.isEmpty()) first.name = second.name;
	if (first.owner.isEmpty()) first.owner = second.owner;
	if (first.iban.isEmpty()) first.iban = second.iban;
	if (first.bic.isEmpty()) first.bic = second.bic;
	if (first.accountNumber.isEmpty()) first.accountNumber = second.accountNumber;
	if (first.bankCode.isEmpty()) first.bankCode = second.bankCode;
	first.initialBalance += second.initialBalance;
	first.balance += second.balance;
	save(first);

	// combine tags
	std::vector<int> firstTagsIds;
	for (const auto& t: (*db)(select(accTag.tagId).from(accTag).where(accTag.accountId == first.id))) {
		firstTagsIds.push_back(t.tagId);
	}
	auto tagIds = value_list_t<std::vector<int>>(firstTagsIds);
	for (const auto& t : (*db)(select(accTag.tagId).from(accTag).where(accTag.accountId == second.id and accTag.tagId.not_in(tagIds)))) {
		(*db)(insert_into(accTag).set(accTag.tagId = t.tagId, accTag.accountId = first.id));
	}
	(*db)(remove_from(accTag).where(accTag.accountId == second.id));

	// update transfers to use the first account
	(*db)(update(tr).set(tr.fromId = first.id).where(tr.fromId == second.id));
	(*db)(update(tr).set(tr.toId = first.id).where(tr.toId == second.id));

	// clean second account from db
	(*db)(remove_from(acc).where(acc.id == second.id));

	// update cache and view
	// by switching merged  account to the end of cachedAccounts and calling pop_back
	emit beginResetModel();
	{
		// remove from id2Row
		int idx2 = id2Row[second.id];
		id2Row.erase(second.id);
		id2Row[cachedAccounts.back().id] = idx2;
		// remove from cachedAccounts
		std::swap(cachedAccounts[idx2], cachedAccounts.back());
		cachedAccounts.pop_back();
	}
	emit endResetModel();

	emit accountsMerged(first, {second.id});
}

Account& AccountModel::_get(int row) {
	assert_error(row >= 0 && row < cachedAccounts.size());
	return cachedAccounts[row];
}

Account& AccountModel::_getById(int id) {
	assert_error(id >= 0 && id2Row.find(id) != id2Row.end());
	int row = id2Row.at(id);
	return _get(row);
}

void AccountModel::save(const Account& a) {
	db::Account acc;
	(*db)(sqlpp::update(acc).set(acc.isOwn = a.isOwn,
							acc.name = str(a.name),
							acc.owner = str(a.owner),
							acc.iban = str(a.iban),
							acc.bic = str(a.bic),
							acc.accountNumber = str(a.accountNumber),
							acc.bankCode = str(a.bankCode),
							acc.initialBalance = a.initialBalance,
							acc.balance = a.balance
						).where(acc.id == a.id));
}

void AccountModel::emitChanged(int id) {
	int row = id2Row[id];
	emit dataChanged(index(row, 0), index(row, COLUMNS_COUNT -1));
}

void AccountModel::assertValidIndex(const QModelIndex& index) const {
	assert_error(index.column() >= 0 || index.column() < COLUMNS_COUNT, "invalid index column %d (row :%d)", index.column(), index.row());
	assert_error(index.row() >= 0 || index.row() < cachedAccounts.size(), "invalid index row %d (column :%d)", index.row(), index.column());
}
