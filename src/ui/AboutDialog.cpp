#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include <mr/common>
#include <mr/version>
#include <QUrl>
#include <QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);

	ui->version->setText(tr("Version: %1").arg(mr::Version::app().longStr()));
	ui->libversion->setText(tr("mr-lib version: %1").arg(mr::Version::lib().longStr()));
}

AboutDialog::~AboutDialog()
{
	delete ui;
}
