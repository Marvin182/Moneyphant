#include "ColumnChooser.h"

#include <QGridLayout>

QStringList InputTypes = QStringList{"", "id", "date", "amount", "reference", "senderOwnerName", "senderIban", "senderBic", "senderId", "receiverOwnerName", "receiverIban", "receiverBic", "receiverId", "note", "checked", "internal", "tags"};

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
	// assert_error(_columnIndex == 0 || layout->rowCount() == _columnIndex, "non empty layout before ColumnChoosers were constructed: rowCount was %d, columnIndex was %d", layout->rowCount(), _columnIndex);

	layout->addWidget(lHeaderLabel, _columnIndex, 0);
	layout->addWidget(lExampleLabel, _columnIndex, 1);

	for (auto inputType : InputTypes) {
		cbInputType->addItem(inputType);
	}
	connect(cbInputType, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentInputTypeChanged(int)));

	layout->addWidget(cbInputType, _columnIndex, 2);
}

ColumnChooser::~ColumnChooser() {
	delete lHeaderLabel;
	delete lExampleLabel;
	delete cbInputType;
}

void ColumnChooser::set(cqstring headerLine, cqstring exampleLine) {
	lHeaderLabel->setText(headerLine);
	lExampleLabel->setText(exampleLine);
}

cqstring ColumnChooser::inputType() const {
	return InputTypes[inputTypeIndex()];
}

void ColumnChooser::unset() {
	cbInputType->setCurrentIndex(0);
}

void ColumnChooser::unsetIfInputTypeIndex(cqstring inputType) {
	int inputTypeIdx = InputTypes.indexOf(inputType);
	assert_warning(inputTypeIdx > 0);
	if (cbInputType->currentIndex() == inputTypeIdx) {
		unset();
	}
}

void ColumnChooser::setInputType(cqstring inputType) {
	int inputTypeIdx = InputTypes.indexOf(inputType);
	assert_warning(inputTypeIdx >= 0, "invalid input format with input type %s form column %d, resulting in input index %d", cstr(inputType), _columnIndex, inputTypeIdx);
	cbInputType->setCurrentIndex(inputTypeIdx);
}

void ColumnChooser::onCurrentInputTypeChanged(int inputTypeIndex) {
	emit inputTypeChanged(_columnIndex, InputTypes[inputTypeIndex]);
}
