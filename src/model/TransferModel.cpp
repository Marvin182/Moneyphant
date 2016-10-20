#include "TransferModel.h"

#include <QFile>
#include <QColor>
#include <mr/common>
#include <date.h>
#include "../util.h"
#include "Currency.h"

constexpr int COLUMNS_COUNT = 7;

SQLPP_ALIAS_PROVIDER(fromName)
SQLPP_ALIAS_PROVIDER(fromIsOwn)
SQLPP_ALIAS_PROVIDER(toName)
SQLPP_ALIAS_PROVIDER(toIsOwn)

TransferModel::TransferModel(Db db, QObject* parent) :
	QAbstractTableModel(parent),
	db(db),
	cachedTransfers(0)
{
	invalidateCache();
}

int TransferModel::rowCount(const QModelIndex& parent) const {
	assert_debug(parent == QModelIndex());
	return cachedTransfers.size();
}

int TransferModel::columnCount(const QModelIndex& parent) const {
	assert_debug(parent == QModelIndex());
	return COLUMNS_COUNT;
}

QVariant TransferModel::data(const QModelIndex& index, int role) const {
	assertValidIndex(index);
	const auto& t = cachedTransfers[index.row()];

	switch (role) {
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
		case Qt::EditRole:
			switch (index.column()) {
				case 0: return t.dateStr();
				case 1: return t.from.name;
				case 2: return t.to.name;
				case 3: return t.reference.isEmpty() ? t.note : t.reference;
				case 4: return QString(t.value);
			}
			break;
		case Qt::CheckStateRole:
			switch (index.column()) {
				case 5:	return t.checked ? Qt::Checked : Qt::Unchecked;
				case 6:	return t.internal ? Qt::Checked : Qt::Unchecked;
			}
			break;
		case Qt::ForegroundRole:
			switch (index.column()) {
				case 4: return t.value.amount >= 0 ? QColor(Qt::green) : QColor(Qt::red);
			}
			break;
		case Qt::UserRole + 1:
			return QString(t.from.name + t.to.name + t.reference);
			break;
	}

	return QVariant();
}

QVariant TransferModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case 0: return tr("Date");
			case 1: return tr("From");
			case 2: return tr("To");
			case 3: return tr("Reference / Note");
			case 4: return tr("Amount");
			case 5: return tr("Checked");
			case 6: return tr("Internal");
			default: assert_unreachable();
		}
	}
	
	return QVariant();
}

bool TransferModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	assertValidIndex(index);
	auto& t = cachedTransfers[index.row()];

	db::Transfer tr;
	switch (index.column()) {
		case 5:
			assert_error(value.canConvert<bool>());
			assert_error(value.toBool() != t.checked, "checked state didn't change");
			t.checked = value.toBool();
			(*db)(update(tr).set(tr.checked = t.checked).where(tr.id == t.id));
			break;
		case 6:
			assert_error(value.canConvert<bool>());
			assert_error(value.toBool() != t.internal);
			t.internal = value.toBool();
			(*db)(update(tr).set(tr.internal = t.internal).where(tr.id == t.id));
			break;
		default:
			assert_unreachable();
			break;
	}

	emit dataChanged(index, index);
	return true;
}

Qt::ItemFlags TransferModel::flags(const QModelIndex& index) const {
	assertValidIndex(index);

	auto commonFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	switch (index.column()) {
		case 5:
		case 6:
			return commonFlags | Qt::ItemIsUserCheckable;
		default:
			return commonFlags;
	}
}

void TransferModel::setNote(int transferId, cqstring note) {
	auto& t = _getById(transferId);
	if (t.note == note) {
		return;
	} else {
		t.note = note;
		db::Transfer tr;
		(*db)(update(tr).set(tr.note = str(note)).where(tr.id == transferId));
	}
}

void TransferModel::toggleChecked(const std::vector<int>& transferIds) {
	bool checked = false;
	for (int id : transferIds) {
		if (!getById(id).checked) {
			checked = true;
			break;
		}
	}

	// update database
	db::Transfer tr;
	auto ids = sqlpp::value_list_t<std::vector<int>>(transferIds);
	(*db)(update(tr).set(tr.checked = checked).where(tr.id.in(ids)));

	// update cache and inform UI
	for (int id : transferIds) {
		_getById(id).checked = checked;
	}

	emit dataChanged(createIndex(0, 5), createIndex(cachedTransfers.size() - 1, 5), {Qt::CheckStateRole});
}

void TransferModel::toggleInternal(const std::vector<int>& transferIds) {
	bool internal = false;
	for (int id : transferIds) {
		if (!getById(id).internal) {
			internal = true;
			break;
		}
	}

	// update database
	db::Transfer tr;
	auto ids = sqlpp::value_list_t<std::vector<int>>(transferIds);
	(*db)(update(tr).set(tr.internal = internal).where(tr.id.in(ids)));

	// update cache and inform UI
	for (int id : transferIds) {
		_getById(id).internal = internal;
	}

	emit dataChanged(createIndex(0, 6), createIndex(cachedTransfers.size() - 1, 6), {Qt::CheckStateRole});
}

void TransferModel::remove(const std::vector<int>& transferIds) {
	// update database
	db::Transfer tr;
	auto ids = sqlpp::value_list_t<std::vector<int>>(transferIds);
	(*db)(remove_from(tr).where(tr.id.in(ids)));

	invalidateCache();
}

void TransferModel::exportTransfers(cqstring path, const std::vector<int>& transferIds) const {
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		assert_error(false, "Could not open export file '%s'", cstr(path));
	}
	for (int id : transferIds) {
		const auto& t = getById(id);
		file.write(QString("%1;%2;%3;%4;%5\n").arg(t.dateStr()).arg(t.from.name).arg(t.to.name).arg(t.reference).arg(t.value.amount / 100.0, 2, 'f', 2).toUtf8());
	}	
}

void TransferModel::invalidateCache() {
	emit beginResetModel();

	db::Account acc;
	db::Transfer tr;
	auto accFrom = acc.as(fromName);
	auto accTo = acc.as(toName);
	
	int rowCount = (*db)(select(count(tr.id)).from(tr).unconditionally()).front().count;
	cachedTransfers.clear();
	cachedTransfers.reserve(rowCount);

	int row = 0;
	for (const auto& t : (*db)(select(all_of(tr),
								accFrom.name.as(fromName), accFrom.isOwn.as(fromIsOwn), accTo.name.as(toName), accTo.isOwn.as(toIsOwn))
								.from(tr.join(accFrom).on(tr.fromId == accFrom.id).join(accTo).on(tr.toId == accTo.id)).unconditionally())) {
		id2Row[t.id] = row++;
		auto cur = Currency::get(t.currency);
		cachedTransfers.push_back({(int)t.id,
								date::year_month_day{t.ymd},
								Transfer::Acc(t.fromId, qstr(t.fromName), t.fromIsOwn),
								Transfer::Acc(t.toId, qstr(t.toName), t.toIsOwn),
								qstr(t.reference), {t.amount.value(), cur}, qstr(t.note), t.checked, t.internal
		});
	}

	emit endResetModel();
}

void TransferModel::onAccountsMerged(const Account& merged, QSet<int> mergedIds) {
	emit beginResetModel();

	for (auto& t : cachedTransfers) {
		if (mergedIds.contains(t.from.id)) {
			t.from.id = merged.id;
			t.from.name = merged.name;
		}
		if (mergedIds.contains(t.to.id)) {
			t.to.id = merged.id;
			t.to.name = merged.name;
		}
	}

	emit endResetModel();
}

void TransferModel::createBackup(const QString& path) {
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		assert_error(false, "Could not open backup file '%s'", cstr(path));
	}
	for (const auto& t : cachedTransfers) {
		file.write((QString("%1;%2;%3;%4;%5;%6;%7;%8").arg(t.id).arg(t.dateStr("yyyy-MM-dd")).arg(t.from.id).arg(t.from.name).arg(t.to.id).arg(t.to.name).arg(t.reference).arg(t.value.amount) + QString(";%1;%2;%3\n").arg(t.note).arg(t.checked ? "1" : "0").arg(t.internal ? "1" : "0")).toLocal8Bit());
	}
}

const Transfer& TransferModel::get(int row) const {
	assert_error(row >= 0 && row < cachedTransfers.size(), "row %d not cached", row);
	return cachedTransfers[row];
}

Transfer& TransferModel::_get(int row) {
	assert_error(row >= 0 && row < cachedTransfers.size(), "row %d not cached", row);
	return cachedTransfers[row];
}

const Transfer& TransferModel::getById(int id) const {
	assert_error(id >= 0 && id2Row.find(id) != id2Row.end(), "id %d not found in cache", id);
	int row = id2Row.at(id);
	return get(row);
}

Transfer& TransferModel::_getById(int id) {
	assert_error(id >= 0 && id2Row.find(id) != id2Row.end(), "id %d not found in cache", id);
	int row = id2Row.at(id);
	return _get(row);
}

void TransferModel::save(const Transfer& t) {
	db::Transfer tr;
	(*db)(update(tr).set(tr.ymd = t.dayPoint(),
							tr.fromId = t.from.id,
							tr.toId = t.to.id,
							tr.reference = str(t.reference),
							tr.amount = t.value.amount,
							tr.currency = t.value.currency->isoCode,
							tr.note = str(t.note),
							tr.checked = t.checked,
							tr.internal = t.internal
						).where(tr.id == t.id));
}

void TransferModel::emitChanged(int id) {
	int row = id2Row[id];
	emit dataChanged(index(row, 0), index(row, COLUMNS_COUNT - 1));
}


void TransferModel::assertValidIndex(const QModelIndex& index) const {
	assert_error(index.column() >= 0 || index.column() < COLUMNS_COUNT, "invalid index column %d (row :%d)", index.column(), index.row());
	assert_error(index.row() >= 0 || index.row() < cachedTransfers.size(), "invalid index row %d (column :%d)", index.row(), index.column());
}

