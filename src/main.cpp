#include <QApplication>
#include <QMessageBox>
#include <mr/common>
#include <mr/qt>
#include "ui/MainWindow.h"
#include <iostream>

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

int main(int argc, char* argv[]) {
	QApplication::setOrganizationName("Moneyphant");
	QApplication::setOrganizationDomain("de.mr.moneyphant");
	QApplication::setApplicationName("Moneyphant");

	mr::assert::addAssertHandler(customAssertHandler);
	mr::assert::init();
	mr::log::init();

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
