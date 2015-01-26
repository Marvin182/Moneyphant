#include "TagHelper.h"
#include <algorithm>

TagHelper::TagHelper(Db db, QObject* parent) :
	QObject(parent),
	db(db),
	_accountIds(0),
	_transferIds(0)
{}

int TagHelper::tagId(cqstring name) {
	auto tags = db->run(select(tag.id).from(tag).where(tag.name == str(name)));
	if (!tags.empty()) {
		assert_error(tags.front().id >= 0, "database returned invalid id %d for tag '%s'", (int)tags.front().id, cstr(name));
		return tags.front().id;
	}

	int id = db->run(insert_into(tag).set(tag.name = str(name)));
	assert_error(id >= 0, "inserting tag '%s' failed (id: %d)", cstr(name), id);
	return id;
}

QStringList TagHelper::tagsFromAccounts(int fromId, int toId) {
	db::Tag tag;
	db::AccountTag accTag;

	QStringList tags;
	for (const auto& t : db->run(select(tag.name).from(tag, accTag).where(tag.id == accTag.tagId and (accTag.accountId == fromId or accTag.accountId == toId)))) {
		tags << qstr(t.name);
	}
	return tags;
}

void TagHelper::addAccountId(int accountId) {
	assert_error(accountId >= 0, "invalid account id %d", accountId);
	if (_accountIds.size() == 1 && _accountIds.front() == accountId) {
		return;			
	}
	assert_error(std::find(_accountIds.begin(), _accountIds.end(), accountId) == _accountIds.end(), "account id %d was already added", accountId);
	_accountIds.push_back(accountId);
}

void TagHelper::removeAccountId(int accountId) {
	assert_error(accountId >= 0, "invalid account id %d", accountId);
	auto it = std::find(_accountIds.begin(), _accountIds.end(), accountId);
	assert_error(it != _accountIds.end(), "account id %d cannot be removed", accountId);
	_accountIds.erase(it, it + 1);
}

void TagHelper::addTransferId(int transferId) {
	assert_error(transferId >= 0, "invalid transferId id %d", transferId);
	if (_transferIds.size() == 1 && _transferIds.front() == transferId) {
		return;			
	}
	assert_error(std::find(_transferIds.begin(), _transferIds.end(), transferId) == _transferIds.end(), "transfer id %d was already added", transferId);
	_transferIds.push_back(transferId);
}

void TagHelper::removeTransferId(int transferId) {
	assert_error(transferId >= 0, "invalid transferId id %d", transferId);
	auto it = std::find(_transferIds.begin(), _transferIds.end(), transferId);
	assert_error(it != _transferIds.end(), "transfer id %d cannot be removed", transferId);
	_transferIds.erase(it, it + 1);
}

QStringList TagHelper::getAccountTags(TagHelper::IdList& customAccIds) {
	std::vector<int> accIds = customAccIds.empty() ? accountIds() : customAccIds;
	for (int id : accIds) { assert_error(id >= 0, "invalid account id %d", id); }

	QStringList tags;

	if (accIds.size() == 1) {
		for (const auto& t : db->run(select(tag.name).from(tag, accTag).where(tag.id == accTag.tagId and accTag.accountId == accIds.front()))) {
			tags << qstr(t.name);
		}
	} else {
		int seenCount = 0;
		int currentTagId = -1;
		QString currentTagName("");

		auto ids = value_list_t<std::vector<int>>(accIds);
		for (const auto& t : db->run(select(tag.id, tag.name).from(tag, accTag).where(tag.id == accTag.tagId and accTag.accountId.in(ids)).order_by(tag.id))) {
			if (t.id == currentTagId) {
				seenCount++;
			} else {
				if (seenCount == accIds.size()) {
					tags << currentTagName;
				}
				seenCount = 1;
				currentTagId = t.id;
				currentTagName = qstr(t.name);
			}
		}

		if (seenCount == accIds.size()) {
			tags << currentTagName;
		}
	}

	return tags;
}

void TagHelper::addAccountTags(QStringList tags, const std::vector<int>& customAccIds) {
	auto accIds = customAccIds.empty() ? accountIds() : customAccIds;

	for (int accId : accIds) {
		assert_error(accId >= 0, "invalid account id %d", accId);
		for (const auto& t : tags) {
			assert_error(!t.isEmpty(), "empty tag name");
			int tid = tagId(t);
			assert_error(tid >= 0, "invalid tag id %d for '%s'", tid, cstr(t));
			bool exists = db->run(select(count(accTag.tagId)).from(accTag).where(accTag.tagId == tid and accTag.accountId == accId)).front().count == 1;
			if (!exists) {
				db->run(insert_into(accTag).set(accTag.tagId = tid, accTag.accountId = accId));
			}
		}
	}
}

void TagHelper::removeAccountTags(QStringList tags, const std::vector<int>& customAccIds) {
	auto accIds = customAccIds.empty() ? accountIds() : customAccIds;

	for (int accId : accIds) {
		assert_error(accId >= 0, "invalid account id %d", accId);
		for (const auto& t : tags) {
			assert_error(!t.isEmpty(), "empty tag name");
			int tid = tagId(t);
			assert_error(tid >= 0, "invalid tag id %d for '%s'", tid, cstr(t));
			assert_error(db->run(select(count(accTag.tagId)).from(accTag).where(accTag.tagId == tid and accTag.accountId == accId)).front().count == 1, "account tag (account id: %d, tag id: %d, tag name: '%s') does not exists", accId, tid, cstr(t));
			db->run(remove_from(accTag).where(accTag.tagId == tid and accTag.accountId == accId));
		}
	}
}

QStringList TagHelper::getTransferTags(TagHelper::IdList& customTrId) {
	std::vector<int> trIds = customTrId.empty() ? accountIds() : customTrId;
	for (int id : trIds) { assert_error(id >= 0, "invalid transfer id %d", id); }

	QStringList tags;

	if (trIds.size() == 1) {
		for (const auto& t : db->run(select(tag.name).from(tag, trTag).where(tag.id == trTag.tagId and trTag.transferId == trIds.front()))) {
			tags << qstr(t.name);
		}
	} else {
		int seenCount = 0;
		int currentTagId = -1;
		QString currentTagName("");

		auto ids = value_list_t<std::vector<int>>(trIds);
		for (const auto& t : db->run(select(tag.id, tag.name).from(tag, trTag).where(tag.id == trTag.tagId and trTag.transferId.in(ids)).order_by(tag.id))) {
			if (t.id == currentTagId) {
				seenCount++;
			} else {
				if (seenCount == trIds.size()) {
					tags << currentTagName;
				}
				seenCount = 1;
				currentTagId = t.id;
				currentTagName = qstr(t.name);
			}
		}
		if (seenCount == trIds.size()) {
			tags << currentTagName;
		}
	}

	return tags;
}

void TagHelper::addTransferTags(QStringList tags, const std::vector<int>& customTrId) {
	auto trIds = customTrId.empty() ? transferIds() : customTrId;

	for (int trId : trIds) {
		assert_error(trId >= 0, "invalid transfer id %d", trId);
		for (const auto& t : tags) {
			assert_error(!t.isEmpty(), "empty tag name");
			int tid = tagId(t);
			assert_error(tid >= 0, "invalid tag id %d for '%s'", tid, cstr(t));
			bool exists = db->run(select(count(trTag.tagId)).from(trTag).where(trTag.tagId == tid and trTag.transferId == trId)).front().count == 1;
			if (!exists) {
				db->run(insert_into(trTag).set(trTag.tagId = tid, trTag.transferId = trId));
			}
		}
	}
}

void TagHelper::removeTransferTags(QStringList tags, const std::vector<int>& customTrId) {
	auto trIds = customTrId.empty() ? transferIds() : customTrId;

	for (int trId : trIds) {
		assert_error(trId >= 0, "invalid transfer id %d", trId);
		for (const auto& t : tags) {
			assert_error(!t.isEmpty(), "empty tag name");
			int tid = tagId(t);
			assert_error(tid >= 0, "invalid tag id %d for '%s'", tid, cstr(t));
			assert_error(db->run(select(count(trTag.tagId)).from(trTag).where(trTag.tagId == tid and trTag.transferId == trId)).front().count == 1, "transfer tag (transfer id: %d, tag id: %d, tag name: '%s') does not exists", trId, tid, cstr(t));
			db->run(remove_from(trTag).where(trTag.tagId == tid and trTag.transferId == trId));
		}
	}
}
