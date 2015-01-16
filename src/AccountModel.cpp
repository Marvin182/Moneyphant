#include "AccountModel.h"
#include "db.h"
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
	Q_UNUSED(parent);
	return cachedAccounts.size();
}

int AccountModel::columnCount(const QModelIndex& parent) const {
	Q_UNUSED(parent);
	return COLUMNS_COUNT;
}

QVariant AccountModel::data(const QModelIndex& index, int role) const {
	assert(index.row() >= 0 && index.row() < cachedAccounts.size());
	assert(index.column() >= 0 && index.column() < COLUMNS_COUNT);
	const auto& a = cachedAccounts[index.row()];

	switch (role) {
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
		case Qt::EditRole:
			switch (index.column()) {
				case 1: return a.name;
				case 2: return a.owner;
				case 3: return a.iban.isEmpty() ? a.accountNumber : a.iban;
				case 4: return a.bic.isEmpty() ? a.bankCode : a.bic;
				case 5: return a.accountNumber;
				case 6: return a.bankCode;
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
			default: assert(false && "code should not be reached");
		}
	}
	
	return QVariant();
}

bool AccountModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	assert(index.row() >= 0 && index.row() < cachedAccounts.size());
	auto& a = cachedAccounts[index.row()];

	assert(index.column() >= 0 && index.column() < COLUMNS_COUNT);
	
	db::Account acc;
	if (index.column() == 0) {
		assert(value.canConvert<bool>());
		assert(value.toBool() != a.isOwn);
		a.isOwn = value.toBool();
		db->run(update(acc).set(acc.isOwn = a.isOwn).where(acc.id == a.id));
	} else {
		switch (index.column()) {
			case 1:
				a.name = value.toString();
				break;
			default:
				assert(false);
				return false;
		}
	}

	db->run(update(acc).set(acc.isOwn = a.isOwn,
							acc.name = str(a.name),
							acc.owner = str(a.owner),
							acc.iban = str(a.iban),
							acc.bic = str(a.bic),
							acc.accountNumber = str(a.accountNumber),
							acc.bankCode = str(a.bankCode)
						).where(acc.id == a.id));

	emit dataChanged(index, index);
	return true;
}

Qt::ItemFlags AccountModel::flags(const QModelIndex& index) const {
	auto commonFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	switch (index.column()) {
		case 0:
			return commonFlags | Qt::ItemIsUserCheckable;
		case 1:
			return commonFlags | Qt::ItemIsEditable;
		default:
			return commonFlags;
	}
}

const Account& AccountModel::operator[](int id) const {
	assert(id >= 0 && id2Row.find(id) != id2Row.end());
	int row = id2Row.at(id);
	return get(row);
}

const Account& AccountModel::get(int row) const {
	assert(row >= 0 && row < cachedAccounts.size());
	return cachedAccounts[row];
}

void AccountModel::reloadCache() {
	emit beginResetModel();

	db::Account acc;
	int rowCount = db->run(select(count(acc.id)).from(acc).where(true)).front().count;
	cachedAccounts.clear();
	cachedAccounts.reserve(rowCount);

	int row = 0;
	for (const auto& a : db->run(select(all_of(acc)).from(acc).where(true).order_by(acc.isOwn.desc(), acc.name))) {
		id2Row[a.id] = row++;
		cachedAccounts.push_back({(int)a.id, a.isOwn, qstr(a.name), qstr(a.owner), qstr(a.iban), qstr(a.bic), qstr(a.accountNumber), qstr(a.bankCode)});
	}

	emit endResetModel();
}

void AccountModel::createBackup(const QString& path) {
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		assert(false && "Could not open accounts backup file");
	}
	for (const auto& a : cachedAccounts) {
		file.write(QString("%1;%2;%3;%4;%5;%6;%7;%8\n").arg(a.id).arg(a.isOwn ? '1' : '0').arg(a.name).arg(a.owner).arg(a.iban).arg(a.bic).arg(a.accountNumber).arg(a.bankCode).toLocal8Bit());
	}
}

void AccountModel::mergeAccounts(int firstId, int secondId) {
	auto& first = accountById(firstId);
	auto& second = accountById(secondId);

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
	db->run(update(acc).set(acc.isOwn = first.isOwn,
							acc.name = str(first.name),
							acc.owner = str(first.owner),
							acc.iban = str(first.iban),
							acc.bic = str(first.bic),
							acc.accountNumber = str(first.accountNumber),
							acc.bankCode = str(first.bankCode)
						).where(acc.id == first.id));

	// combine tags
	std::vector<int> firstTagsIds;
	for (const auto& t: db->run(select(accTag.tagId).from(accTag).where(accTag.accountId == first.id))) {
		firstTagsIds.push_back(t.tagId);
	}
	auto tagIds = value_list_t<std::vector<int>>(firstTagsIds);
	for (const auto& t : db->run(select(accTag.tagId).from(accTag).where(accTag.accountId == second.id and accTag.tagId.not_in(tagIds)))) {
		db->run(insert_into(accTag).set(accTag.tagId = t.tagId, accTag.accountId = first.id));
	}
	db->run(remove_from(accTag).where(accTag.accountId == second.id));

	// update transfers to use the first account
	db->run(update(tr).set(tr.fromId = first.id).where(tr.fromId == second.id));
	db->run(update(tr).set(tr.toId = first.id).where(tr.toId == second.id));

	// clean second account from db
	db->run(remove_from(acc).where(acc.id == second.id));

	reloadCache(); // yes, too expensive, but I don't feel like writing code to update cachedAccounts and id2Row and view
}

Account& AccountModel::accountById(int id) {
	assert(id >= 0 && id2Row.find(id) != id2Row.end());
	int row = id2Row.at(id);
	assert(row >= 0 && row < cachedAccounts.size());
	return cachedAccounts[row];
}

