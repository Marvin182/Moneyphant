#include "assert.h"
#include <iostream>
#include <QFile>
#include <QDateTime>
#include <QMessageBox>
#include <QApplication>

using namespace pempek::assert::implementation;

const char* ErrorLogFileName = "errors.txt";

AssertAction::AssertAction onAssert(const char* file,
									int line,
									const char* function,
									const char* expression,
									int level,
									const char* message) {
	const char* msg = message == nullptr ? "" : message;

	QFile errorLog(ErrorLogFileName);
	if (errorLog.open(QIODevice::Append | QIODevice::Text)) {
		auto dt = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ": Assertion failed!\n").toUtf8();
		errorLog.write(dt);
		errorLog.write(QString("\texpression: %1\n").arg(expression).toUtf8());
		errorLog.write(QString("\tmessage: %1 (level %2)\n").arg(msg).arg(level).toUtf8());
		errorLog.write(QString("\tfunction %1 in file: %2 line %3\n").arg(function).arg(file).arg(line).toUtf8());
		errorLog.close();
	} else {
		std::cerr << "failed to open error log file '" << ErrorLogFileName << "'" << std::endl;
	}

	// write message to output
	std::cerr << "Assertion failed: " << expression;
	if (message != nullptr) {
		std::cerr << "\nmessage: " << message;
	}
	std::cerr << "\n\tfunction: " << function << "\n\tfile: " << file << " line " << line << std::endl;

	// display error dialog
	QMessageBox msgBox(QMessageBox::Warning, "Moneyphant encountered an error.", "Moneyphant encountered a problem. To prevent any further errors or damage to your data it will shut down.\nYou may restart Moneyphant and continue.");
	if (message != nullptr) {
		msgBox.setInformativeText(QString("Details: %1").arg(message));
	}
	msgBox.setStandardButtons(QMessageBox::Close);
	msgBox.setDefaultButton(QMessageBox::Close);
	int ret = msgBox.exec();

	// TODO: send error report

	// display error dialog and terminate
	switch (ret) {
		case QMessageBox::Close:
			QApplication::exit(0);
			return AssertAction::AssertAction::None;
			break;
		default:
			assert_error(false, "Reached unexpected code path.");
	}

	return AssertAction::AssertAction::Abort;
}

void initAssertHandler() {
	setAssertHandler(onAssert);
}
