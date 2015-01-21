#ifndef GLOBALS_VERSION_H
#define GLOBALS_VERSION_H

#include <QString>

struct Version {
	int major;
	int minor;
	int revision;
	QString status;
	QString commitId;

	static const Version& get();

	int asNumber() const;

	QString shortStr() const;
	QString longStr() const;

private:
	Version();
};

#endif // GLOBALS_VERSION_H
