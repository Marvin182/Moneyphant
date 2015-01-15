#ifndef TAG_H
#define TAG_H

#include "Global.h"

struct Tag {
	int accountId;
	int transferId;
	QString name;

	Tag(int accountId, int transferId, string name);
};

std::ostream& operator<<(std::ostream& os, const Tag& a);

#endif // TAG_H