#include "DataChooser.h"

#include <QGridLayout>

DataChooser::DataChooser(int row, cqstring headerLine, cqstring exampleLine, QWidget* parent) :
	QObject(parent),
	_row(row)
{
	assert_error(_row >= 0 && _row < 1000, "invalid row %d", _row);
	assert_error(parent != nullptr);
	auto layout = (QGridLayout*)parent->layout();
	assert_error(layout != nullptr);
	assert_error(_row == 0 || layout->rowCount() == _row, "non empty layout: rowCount was %d, row was %d", layout->rowCount(), _row);

	headerLabel = new QLabel(headerLine.left(50), parent);
	exampleLabel = new QLabel(exampleLine.left(50), parent);
	layout->addWidget(headerLabel, _row, 0);
	layout->addWidget(exampleLabel, _row, 1);

	comboBox = new QComboBox(parent);
	QStringList items{"", tr("ID"), tr("Date"), tr("Amount"), tr("Reference"), tr("Sender Owner Name"), tr("Sender IBAN"), tr("Sender BIC"), tr("Sender E-Mail"), tr("Sender ID"), tr("Receiver Owner Name"), tr("Receiver IBAN"), tr("Receiver BIC"), tr("Receiver E-Mail"), tr("Receiver ID"), tr("Note"), tr("Checked")};
	comboBox->addItems(items);
	connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
	layout->addWidget(comboBox, _row, 2);
}

DataChooser::~DataChooser() {
	delete headerLabel;
	delete exampleLabel;
	delete comboBox;
}

void DataChooser::unsetIndex(int index) {
	if (comboBox->currentIndex() == index) {
		comboBox->setCurrentIndex(0);
	}
}

void DataChooser::load(const std::vector<int>& format) {
	assert_error(comboBox != nullptr);
	assert_error(_row < (int)format.size(), "invalid format for row %d (format size was %d", _row, (int)format.size());
	int idx = format[_row];
	assert_error(idx >= 0 && idx < comboBox->maxCount(), "invalid index %d in format, max count of combobox is %d", idx, comboBox->maxCount());
	comboBox->setCurrentIndex(idx);
}

void DataChooser::save(std::vector<int>& format) {
	assert_error(comboBox != nullptr);
	assert_error(_row < (int)format.size(), "invalid format for row %d (format size was %d", _row, (int)format.size());
	format[_row] = comboBox->currentIndex();
}

void DataChooser::onCurrentIndexChanged(int index) {
	emit columnChanged(_row, index);
}
