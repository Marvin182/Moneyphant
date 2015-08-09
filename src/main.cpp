#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <mr/common>

const char* messageLogFileName = "log.txt";

AssertAction::AssertAction customAssertHandler(const char* file,
									int line,
									const char* function,
									const char* expression,
									int level,
									const char* message) {
	if (level < 128) {
		return AssertAction::AssertAction::None;
	}

	// display error dialog
	QMessageBox msgBox(QMessageBox::Warning, "Moneyphant encountered an error.", "Moneyphant encountered a problem. To prevent any further errors or damage to your data it will shut down.\nYou may restart Moneyphant and continue.");
	if (message != nullptr) {
		msgBox.setInformativeText(QString("Details: %1").arg(message));
	}
	msgBox.setStandardButtons(QMessageBox::Close);
	msgBox.setDefaultButton(QMessageBox::Close);
	int ret = msgBox.exec();
	assert_debug(ret == QMessageBox::Close);

	return AssertAction::AssertAction::Abort;
}

QString messageTypeToStr(QtMsgType type) {
	switch (type) {
		case QtDebugMsg: return "Debug";
		case QtInfoMsg: return "Info";
		case QtWarningMsg: return "Warning";
		case QtCriticalMsg: return "Critical";
		case QtFatalMsg: return "Fatal";
		case QtSystemMsg: return "System";
	}
	assert_unreachable();
	return "";
}

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	// print to standard output
	std::cout << QString(" %1: %2\n").arg(messageTypeToStr(type)).arg(msg) << std::endl;

	// write message into log file
	QFile messageLog(messageLogFileName);
	if (messageLog.open(QIODevice::Append | QIODevice::Text)) {
		auto dt = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).toUtf8();
		messageLog.write(dt);
		messageLog.write(QString(" %1: %2\n").arg(messageTypeToStr(type)).arg(msg).toUtf8());
		messageLog.close();
	}
}
 
int main(int argc, char *argv[]) {
	QApplication::setOrganizationName("Moneyphant");
	QApplication::setOrganizationDomain("moneyphant");
	QApplication::setApplicationName("Moneyphant");

	mr::assert::setCustomAssertHandler(customAssertHandler);
	mr::assert::initAssertHandler();

	qInstallMessageHandler(customMessageHandler);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
