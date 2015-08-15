#include "AccountModel.h"
#include "sql.h"
#include <QFile>

constexpr int COLUMNS_COUNT = 5;

AccountModel::AccountModel(Db db, QObject* parent) :
	QAbstractTableModel(parent),
	db(db),
	cachedAccounts(0)
{
	reloadCache();
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
				case 1: return a.name;
				case 2: return a.owner;
				case 3: return a.iban.isEmpty() ? a.accountNumber : a.formattedIban();
				case 4: return a.bic.isEmpty() ? mr::separateGroups(a.bankCode, 3, ' ') : a.bic;
				case 5: return a.accountNumber;
				case 6: return a.bankCode;
				// case 7: // tags
				case 8: return currency(a.initialBalance);
			}
			break;
		case Qt::CheckStateRole:
			switch (index.column()) {
				case 0:	return a.isOwn ? Qt::Checked : Qt::Unchecked;
			}
			break;
		case Qt::UserRole + 1:
			return a.name + a.owner + a.iban;
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
			case 0: return tr("Own");
			case 1: return tr("Name");
			case 2: return tr("Owner");
			case 3: return tr("IBAN / Account Number");
			case 4: return tr("BIC / Bank Code");
			case 5: return tr("Account Number");
			case 6: return tr("Bank Code");
			case 7: return tr("Tags");
			case 8: return tr("Initial Balance");
			default: assert_unreachable();
		}
	}
	
	return QVariant();
}

bool AccountModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	assertValidIndex(index);
	auto& a = _get(index.row());
	
	db::Account acc;
	if (index.column() == 0) {
		assert_error(value.canConvert<bool>());
		assert_error(value.toBool() != a.isOwn);
		a.isOwn = value.toBool();
		(*db)(update(acc).set(acc.isOwn = a.isOwn).where(acc.id == a.id));
	} else {
		switch (index.column()) {
			case 1:
				assert_error(value.canConvert<QString>());
				a.name = value.toString();
				break;
			case 2:
				assert_error(value.canConvert<QString>());
				a.owner = value.toString();
				break;
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

	auto commonFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	switch (index.column()) {
		case 0:
			return commonFlags | Qt::ItemIsUserCheckable;
		case 1:
		case 2:
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


void AccountModel::reloadCache() {
	emit beginResetModel();

	db::Account acc;
	int rowCount = (*db)(select(count(acc.id)).from(acc).where(true)).front().count;
	cachedAccounts.clear();
	cachedAccounts.reserve(rowCount);

	int row = 0;
	for (const auto& a : (*db)(select(all_of(acc)).from(acc).where(true).order_by(acc.isOwn.desc(), acc.name.asc()))) {
		id2Row[a.id] = row++;
		cachedAccounts.push_back({(int)a.id, a.isOwn, qstr(a.name), qstr(a.owner), qstr(a.iban), qstr(a.bic), qstr(a.accountNumber), qstr(a.bankCode), (int)a.initialBalance});
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
	(*db)(update(acc).set(acc.isOwn = first.isOwn,
							acc.name = str(first.name),
							acc.owner = str(first.owner),
							acc.iban = str(first.iban),
							acc.bic = str(first.bic),
							acc.accountNumber = str(first.accountNumber),
							acc.bankCode = str(first.bankCode)
						).where(acc.id == first.id));

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

	reloadCache(); // yes, too expensive, but I don't feel like writing code to update cachedAccounts and id2Row and view
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

void AccountModel::assertValidIndex(const QModelIndex& index) const {
	assert_error(index.column() >= 0 || index.column() < COLUMNS_COUNT, "invalid index column %d (row :%d)", index.column(), index.row());
	assert_error(index.row() >= 0 || index.row() < cachedAccounts.size(), "invalid index row %d (column :%d)", index.row(), index.column());
}
