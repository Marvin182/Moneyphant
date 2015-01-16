#ifndef TAG_HELPER_H
#define TAG_HELPER_H

#include "Global.h"
#include "db.h"
#include <vector>
#include <QStringList>

class TagHelper : public QObject {
	Q_OBJECT

public:
	using IdList = const std::vector<int>;

	TagHelper(Db db, QObject* parent = 0);

	IdList& accountIds() const { return _accountIds; }
	IdList& transferIds() const { return _transferIds; }

	int tagId(string name);

public slots:
	void setAccountIds(IdList& accIds) { _accountIds = accIds; }
	void setTransferIds(IdList& trIds) { _transferIds = trIds; }

	void addAccountId(int accountId);
	void removeAccountId(int accountId);
	
	QStringList getAccountTags(IdList& customAccIds = std::vector<int>(0));
	void addAccountTags(QStringList tags, const std::vector<int>& customAccIds = std::vector<int>(0));
	void removeAccountTags(QStringList tags, const std::vector<int>& customAccIds = std::vector<int>(0));

	// [[deprecated("Not used any more, will be removed soon")]]
	// void syncAccountTags(const QStringList& tags, int accId = std::vector<int>(0));

protected:
	Db db;

	db::Tag tag;
	db::AccountTag accTag;
	db::TransferTag trTag;

	std::vector<int> _accountIds;
	std::vector<int> _transferIds;
};

#endif // TAG_HELPER_H
