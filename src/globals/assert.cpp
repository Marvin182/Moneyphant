#include "assert.h"
#include <iostream>
#include <QFile>
#include <QDateTime>
#include <QMessageBox>
#include <QApplication>

using namespace pempek::assert::implementation;

const char* ErrorLogFileName = "errors.txt";

const char* level2Str(int level) {
	switch (level) {
		case AssertLevel::Warning: return "warning";
		case AssertLevel::Debug: return "debug";
		case AssertLevel::Error: return "error";
		case AssertLevel::Fatal: return "fatal";
		default: assert_unreachable();
	}
	return ""; // just to prevent a compiler warning
}

AssertAction::AssertAction onAssert(const char* file,
									int line,
									const char* function,
									const char* expression,
									int level,
									const char* message) {
	const char* msg = message == nullptr ? "" : message;

	bool logError = true;
	bool writeToOutput = true;
	bool informUser = level > AssertLevel::Debug;
	bool exitApplication = informUser;

	if (logError) {
		QFile errorLog(ErrorLogFileName);
		if (errorLog.open(QIODevice::Append | QIODevice::Text)) {
			auto dt = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ": Assertion failed!\n").toUtf8();
			errorLog.write(dt);
			errorLog.write(QString("\texpression: %1\n").arg(expression).toUtf8());
			errorLog.write(QString("\tmessage: %1 (level %2 (%3))\n").arg(msg).arg(level2Str(level)).arg(level).toUtf8());
			errorLog.write(QString("\tfunction %1 in file: %2 line %3\n").arg(function).arg(file).arg(line).toUtf8());
			errorLog.close();
		} else {
			std::cerr << "failed to open error log file '" << ErrorLogFileName << "'" << std::endl;
		}
	}

	if (writeToOutput) {
		// write message to output
		std::cerr << "Assertion failed: " << expression << "\n";
		if (message != nullptr) {
			std::cerr << "\tmessage: " << message << "(level " << level2Str(level) << ", " << level << ")\n";
		} else {
			std::cerr << "\tlevel " << level2Str(level) << ", " << level << ")\n";
		}
		std::cerr << "\tfunction: " << function << "\n\tfile: " << file << " line " << line << std::endl;
	}

	if (informUser) {
		// display error dialog
		QMessageBox msgBox(QMessageBox::Warning, "Moneyphant encountered an error.", "Moneyphant encountered a problem. To prevent any further errors or damage to your data it will shut down.\nYou may restart Moneyphant and continue.");
		if (message != nullptr) {
			msgBox.setInformativeText(QString("Details: %1").arg(message));
		}
		msgBox.setStandardButtons(QMessageBox::Close);
		msgBox.setDefaultButton(QMessageBox::Close);
		int ret = msgBox.exec();
		assert_debug(ret == QMessageBox::Close);
	}

	if (exitApplication) {
		QApplication::exit(0);
		return AssertAction::AssertAction::Abort;
	}

	return AssertAction::AssertAction::None;
}

void initAssertHandler() {
	setAssertHandler(onAssert);
}

void assert_unreachable() {
	assert_fatal(false, "Reached unexpected code path.");
}
