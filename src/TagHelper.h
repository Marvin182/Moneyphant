#ifndef TAG_HELPER_H
#define TAG_HELPER_H

#include <vector>
#include <QRegExp>
#include <QStringList>
#include <mr/common>
#include "sql.h"

// TODO: rewrite as Template class
class TagHelper : public QObject {
	Q_OBJECT

public:
	using IdList = const std::vector<int>;

	TagHelper(Db db = nullptr, QObject* parent = 0);

	IdList& accountIds() const { return _accountIds; }
	IdList& transferIds() const { return _transferIds; }

	int tagId(cqstring name);

	QStringList tagsFromAccounts(int fromId, int toId);

	void setDb(Db db) { this->db = db; }

	static QStringList splitTags(cqstring tags) { return tags.split(QRegExp("[\\s,;]+"), QString::SkipEmptyParts); }

public slots:
	void setAccountIds(IdList& accIds) { _accountIds = accIds; }
	void addAccountId(int accountId);
	void removeAccountId(int accountId);

	void setTransferIds(IdList& trIds) { _transferIds = trIds; }
	void addTransferId(int transferId);
	void removeTransferId(int transferId);
	
	QStringList getAccountTags(IdList& customAccIds = std::vector<int>(0));
	void addAccountTags(QStringList tags, const std::vector<int>& customAccIds = std::vector<int>(0));
	void removeAccountTags(QStringList tags, const std::vector<int>& customAccIds = std::vector<int>(0));

	QStringList getTransferTags(IdList& customTrIds = std::vector<int>(0));
	void addTransferTags(QStringList tags, const std::vector<int>& customTrIds = std::vector<int>(0));
	void removeTransferTags(QStringList tags, const std::vector<int>& customTrIds = std::vector<int>(0));
protected:
	Db db;

	db::Tag tag;
	db::AccountTag accTag;
	db::TransferTag trTag;

	std::vector<int> _accountIds;
	std::vector<int> _transferIds;
};

#endif // TAG_HELPER_H
