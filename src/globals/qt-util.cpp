#include "qt-util.h"

#include "assert.h"
#include "string-util.h"
#include <QDir>
#include <QStandardPaths>

#include <iostream>

QString appLocalDataLocation() {
	auto path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
	assert_fatal(!path.isEmpty(), "empty path for app data location directory");

	auto baseDir = QDir::root();
	QDir dir(baseDir.absolutePath() + QDir::separator() + path);
	if (!dir.exists()) {

		bool b = baseDir.mkpath(path);

		assert_fatal(b, "could not create directory '%s' in '%s'", cstr(path), cstr(baseDir.absolutePath()));
		assert_fatal(dir.exists(), "'%s' does still not exists", cstr(dir.absolutePath()));
	}

	return dir.absolutePath();
}
