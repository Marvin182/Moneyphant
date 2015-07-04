#include "ColumnChooser.h"

#include <QGridLayout>

QStringList ColumnChooser::inputTypes = QStringList{"", "id", "date", "amount", "reference", "senderOwnerName", "senderIban", "senderBic", "senderEmail", "senderId", "receiverOwnerName", "receiverIban", "receiverBic", "receiverEmail", "receiverId", "note", "checked"};

ColumnChooser::ColumnChooser(int columnIndex, cqstring headerLine, cqstring exampleLine, QWidget* parent) :
	QObject(parent),
	_columnIndex(columnIndex),
	lHeaderLabel(new QLabel(headerLine.left(50), parent)),
	lExampleLabel(new QLabel(exampleLine.left(50), parent)),
	cbInputType(new QComboBox(parent))
{
	auto layout = static_cast<QGridLayout*>(parent->layout());
	
	assert_error(parent != nullptr);
	assert_error(_columnIndex >= 0 && _columnIndex < 1000, "invalid column index %d", _columnIndex);
	assert_error(layout != nullptr);
	assert_error(_columnIndex == 0 || layout->rowCount() == _columnIndex, "non empty layout before ColumnChoosers were constructed: rowCount was %d, columnIndex was %d", layout->rowCount(), _columnIndex);

	layout->addWidget(lHeaderLabel, _columnIndex, 0);
	layout->addWidget(lExampleLabel, _columnIndex, 1);

	for (auto inType : inputTypes) {
		cbInputType->addItem(tr(cstr(inType))); // TODO
	}
	connect(cbInputType, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentInputTypeChanged(int)));

	layout->addWidget(cbInputType, _columnIndex, 2);
}

ColumnChooser::~ColumnChooser() {
	delete lHeaderLabel;
	delete lExampleLabel;
	delete cbInputType;
}

void ColumnChooser::unsetIfInputTypeIndex(int inputTypeIndex) {
	assert_warning(inputTypeIndex > 0);
	if (cbInputType->currentIndex() == inputTypeIndex) {
		cbInputType->setCurrentIndex(0);
	}
}

void ColumnChooser::load(const InputFormat& format) {
	cqstring inType = format.key(_columnIndex, "");
	int inIndex = inputTypes.indexOf(inType);
	assert_warning(inIndex >= 0, "invalid input format with input type %s form column %d, resulting in input index %d", cstr(inType), _columnIndex, inIndex);
	cbInputType->setCurrentIndex(inIndex);
}

void ColumnChooser::save(InputFormat& format) const {
	if (inputTypeIndex() > 0) {
		format[inputType()] = _columnIndex;
	}
}

void ColumnChooser::onCurrentInputTypeChanged(int inputTypeIndex) {
	emit inputTypeIndexChanged(_columnIndex, inputTypeIndex);
}
