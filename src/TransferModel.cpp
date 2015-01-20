#include "TransferModel.h"
#include "db.h"
#include <QFile>
#include <QColor>

#include <cassert> // TODO remove

constexpr int COLUMNS_COUNT = 7;

SQLPP_ALIAS_PROVIDER(fromName)
SQLPP_ALIAS_PROVIDER(toName)

TransferModel::TransferModel(Db db, QObject* parent) :
	QAbstractTableModel(parent),
	db(db),
	cachedTransfers(0)
{
	reloadCache();
}

int TransferModel::rowCount(const QModelIndex& parent) const {
	Q_UNUSED(parent);
	return cachedTransfers.size();
}

int TransferModel::columnCount(const QModelIndex& parent) const {
	Q_UNUSED(parent);
	return COLUMNS_COUNT;
}

QVariant TransferModel::data(const QModelIndex& index, int role) const {
	assert(index.row() >= 0 && index.row() < cachedTransfers.size());
	assert(index.column() >= 0 && index.column() < COLUMNS_COUNT);
	const auto& t = cachedTransfers[index.row()];

	switch (role) {
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
		case Qt::EditRole:
			switch (index.column()) {
				case 0: return t.dateStr();
				case 1: return t.from.name;
				case 2: return t.to.name;
				case 3: return t.reference;
				case 4: return currency(t.amount);
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
				case 4: return t.amount >= 0 ? QColor(Qt::green) : QColor(Qt::red);
			}
			break;
		case Qt::UserRole + 1:
			return t.from.name + t.to.name + t.reference;
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
			case 3: return tr("Reference");
			case 4: return tr("Amount");
			case 5: return tr("Checked");
			case 6: return tr("Internal");
			default: assert(false && "invalid section");
		}
	}
	
	return QVariant();
}

bool TransferModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	assert(index.row() >= 0 && index.row() < cachedTransfers.size());
	auto& t = cachedTransfers[index.row()];

	assert(index.column() >= 0 && index.column() < COLUMNS_COUNT);
	
	db::Transfer tr;
	switch (index.column()) {
		case 5:
			assert(value.canConvert<bool>());
			assert(value.toBool() != t.checked);
			t.checked = value.toBool();
			db->run(update(tr).set(tr.checked = t.checked).where(tr.id == t.id));
			break;
		case 6:
			assert(value.canConvert<bool>());
			assert(value.toBool() != t.internal);
			t.internal = value.toBool();
			db->run(update(tr).set(tr.internal = t.internal).where(tr.id == t.id));
			break;
		default:
			assert(false && "cannot setDate for read only column");
			break;
	}

	emit dataChanged(index, index);
	return true;
}

Qt::ItemFlags TransferModel::flags(const QModelIndex& index) const {
	auto commonFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	switch (index.column()) {
		case 5:
		case 6:
			return commonFlags | Qt::ItemIsUserCheckable;
		default:
			return commonFlags;
	}
}

const Transfer& TransferModel::operator[](int id) const {
	assert(id >= 0 && id2Row.find(id) != id2Row.end());
	int row = id2Row.at(id);
	return get(row);
}

const Transfer& TransferModel::get(int row) const {
	assert(row >= 0 && row < cachedTransfers.size());
	return cachedTransfers[row];
}

const Transfer& TransferModel::getById(int id) const {
	assert(id >= 0 && id2Row.find(id) != id2Row.end());
	int row = id2Row.at(id);
	return get(row);
}

void TransferModel::setNote(int transferId, cqstring note) {
	auto& t = transferById(transferId);
	if (t.note == note) {
		return;
	} else {
		t.note = note;
		db::Transfer tr;
		db->run(update(tr).set(tr.note = str(note)).where(tr.id == transferId));
	}
}

void TransferModel::setChecked(const std::vector<int>& transferIds, bool checked) {
	// update database
	db::Transfer tr;
	auto ids = value_list_t<std::vector<int>>(transferIds);
	db->run(update(tr).set(tr.checked = checked).where(tr.id.in(ids)));

	// update cache and inform UI
	QVector<int> roles(1, Qt::CheckStateRole);
	for (int id : transferIds) {
		transferById(id).checked = checked;
		int row = id2Row.at(id);
		auto idx = index(row, 5);
		emit dataChanged(idx, idx, roles);
	}
}

void TransferModel::exportTransfers(cqstring path, const std::vector<int>& transferIds) const {
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		assert(false && "Could not open export file");
	}
	for (int id : transferIds) {
		const auto& t = getById(id);
		file.write(QString("%1;%2;%3;%4;%5\n").arg(t.dateStr()).arg(t.from.name).arg(t.to.name).arg(t.reference).arg(t.amount / 100.0, 2, 'f', 2).toUtf8());
	}	
}

void TransferModel::reloadCache() {
	emit beginResetModel();

	db::Account acc;
	db::Transfer tr;
	auto accFrom = acc.as(fromName);
	auto accTo = acc.as(toName);
	
	int rowCount = db->run(select(count(tr.id)).from(tr).where(true)).front().count;
	cachedTransfers.clear();
	cachedTransfers.reserve(rowCount);

	int row = 0;
	for (const auto& t : db->run(select(all_of(tr), accFrom.name.as(fromName), accTo.name.as(toName)).from(tr, accFrom, accTo).
									where(tr.fromId == accFrom.id and tr.toId == accTo.id))) {
		id2Row[t.id] = row++;
		cachedTransfers.push_back({(int)t.id, QDateTime::fromMSecsSinceEpoch(t.date), Transfer::Acc(t.fromId, qstr(t.fromName)), Transfer::Acc(t.toId, qstr(t.toName)), qstr(t.reference), (int)t.amount, qstr(t.note), t.checked, t.internal});
	}

	emit endResetModel();
}

void TransferModel::createBackup(const QString& path) {
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		assert(false && "Could not open accounts backup file");
	}
	for (const auto& t : cachedTransfers) {
		file.write((QString("%1;%2;%3;%4;%5;%6;%7;%8").arg(t.id).arg(t.dateMs()).arg(t.from.id).arg(t.from.name).arg(t.to.id).arg(t.to.name).arg(t.reference).arg(t.amount) + QString(";%1;%2;%3\n").arg(t.note).arg(t.checked ? "1" : "0").arg(t.internal ? "1" : "0")).toLocal8Bit());
	}
}

Transfer& TransferModel::transferById(int id) {
	assert(id >= 0 && id2Row.find(id) != id2Row.end());
	int row = id2Row.at(id);
	assert(row >= 0 && row < cachedTransfers.size());
	return cachedTransfers[row];
}

template <typename F>
void TransferModel::updateTransfer(int transferId, F f) {
	auto& t = transferById(transferId);
	f(t);
	db::Transfer tr;
	db->run(update(tr).set(tr.date = t.dateMs(),
							tr.fromId = t.from.id,
							tr.toId = t.to.id,
							tr.reference = str(t.reference),
							tr.amount = t.amount,
							tr.note = str(t.note),
							tr.checked = t.checked,
							tr.internal = t.internal
						).where(tr.id == transferId));
}
