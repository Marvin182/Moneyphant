#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("Ritter Apps");
	QCoreApplication::setOrganizationDomain("ritter.moneyphant");
	QCoreApplication::setApplicationName("Moneyphant");

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
