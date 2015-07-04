#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include "mr/common.h"
#include <QUrl>
#include <QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);

	ui->version->setText(tr("Version: %1").arg(mr::Version::get().longStr()));
}

AboutDialog::~AboutDialog()
{
	delete ui;
}