#include "Tag.h"
#include <sstream>

Tag::Tag(int accountId, int transferId, string name) :
	accountId(accountId),
	transferId(transferId),
	name(name)
{
	assert(accountId == -1 || transferId == -1);
	assert(accountId >= 0 || transferId >= 0);
}

std::ostream& operator<<(std::ostream& os, const Tag& t) {
	os << "Tag(" << t.accountId << ", " << t.transferId << ", " << t.name << ")";
	return os;
}
