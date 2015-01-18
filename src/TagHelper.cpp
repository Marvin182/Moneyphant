#include "TagHelper.h"
#include <algorithm>

TagHelper::TagHelper(Db db, QObject* parent) :
	QObject(parent),
	db(db),
	_accountIds(0),
	_transferIds(0)
{}

int TagHelper::tagId(string name) {
	auto tags = db->run(select(tag.id).from(tag).where(tag.name == str(name)));
	if (!tags.empty()) {
		assert(tags.front().id >= 0);
		return tags.front().id;
	}

	int id = db->run(insert_into(tag).set(tag.name = str(name)));
	assert(id >= 0);
	return id;
}

QStringList TagHelper::accountTagsOfTransfer(int fromId, int toId) {
	db::Tag tag;
	db::AccountTag accTag;

	QStringList tags;
	for (const auto& t : db->run(select(tag.name).from(tag, accTag).where(tag.id == accTag.tagId and accTag.accountId == fromId or accTag.accountId == toId))) {
		tags << qstr(t.name);
	}
	return tags;
}

void TagHelper::addAccountId(int accountId) {
	assert(accountId >= 0);
	if (_accountIds.size() == 1 && _accountIds.front() == accountId) {
		return;			
	}
	assert(std::find(_accountIds.begin(), _accountIds.end(), accountId) == _accountIds.end());
	_accountIds.push_back(accountId);
}

void TagHelper::removeAccountId(int accountId) {
	assert(accountId >= 0);
	auto it = std::find(_accountIds.begin(), _accountIds.end(), accountId);
	assert(it != _accountIds.end());
	_accountIds.erase(it, it + 1);
}

QStringList TagHelper::getAccountTags(TagHelper::IdList& customAccIds) {
	std::vector<int> accIds = customAccIds.empty() ? accountIds() : customAccIds;
	for (int id : accIds) { assert(id >= 0); }

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
	}

	return tags;
}

void TagHelper::addAccountTags(QStringList tags, const std::vector<int>& customAccIds) {
	auto accIds = customAccIds.empty() ? accountIds() : customAccIds;

	for (int accId : accIds) {
		assert(accId >= 0);
		for (const auto& t : tags) {
			int tid = tagId(t);
			assert(tid >= 0);
			assert(0 == db->run(select(count(accTag.tagId)).from(accTag).where(accTag.tagId == tid and accTag.accountId == accId)).front().count);
			db->run(insert_into(accTag).set(accTag.tagId = tid, accTag.accountId = accId));
		}
	}
}

void TagHelper::removeAccountTags(QStringList tags, const std::vector<int>& customAccIds) {
	auto accIds = customAccIds.empty() ? accountIds() : customAccIds;

	for (int accId : accIds) {
		assert(accId >= 0);
		for (const auto& t : tags) {
			assert(!t.isEmpty());
			int tid = tagId(t);
			assert(tid >= 0);
			assert(1 == db->run(select(count(accTag.tagId)).from(accTag).where(accTag.tagId == tid and accTag.accountId == accId)).front().count);
			db->run(remove_from(accTag).where(accTag.tagId == tid and accTag.accountId == accId));
		}
	}
}
