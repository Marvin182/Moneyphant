#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication::setOrganizationName("Ritter Apps");
	QApplication::setOrganizationDomain("ritter.moneyphant");
	QApplication::setApplicationName("Moneyphant");

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
