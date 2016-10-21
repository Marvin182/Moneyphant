#include "AboutDialog.h"

#include "ui_AboutDialog.h"
#include "../version.h"

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);

	ui->version->setText(tr("Version: %1").arg(version.toString().c_str()));
	ui->libversion->setText(tr("mr-lib version: %1").arg(version.toString().c_str()));
}

AboutDialog::~AboutDialog()
{
	delete ui;
}
