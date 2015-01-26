#include "qt-util.h"

#include "assert.h"
#include "string-util.h"
#include <QDir>
#include <QStandardPaths>

QString appDataLocation() {
	auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	assert_fatal(!path.isEmpty(), "empty path for app data location directory");

	#ifdef Q_OS_OSX
		assert_error(path[0] == '/', "assuming that path starts with '/', '%s' did not", cstr(path));
		path = path.right(path.length() - 1);
		auto baseDir = QDir::home();
	#else
		auto baseDir = QDir::root();
	#endif

	QDir dir(baseDir.absolutePath() + QDir::separator() + path);
	if (!dir.exists()) {
		bool b = baseDir.mkpath(path);

		assert_fatal(b, "could not create directory '%s' in '%s'", cstr(path), cstr(baseDir.absolutePath()));
		assert_fatal(dir.exists(), "'%s' does still not exists", cstr(dir.absolutePath()));
	}

	return dir.absolutePath();
}
