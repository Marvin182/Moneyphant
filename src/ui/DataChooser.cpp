#include "DataChooser.h"

#include <QLabel>
#include <QComboBox>
#include <QGridLayout>

DataChooser::DataChooser(int columnIndex, cqstring headerLine, cqstring exampleLine, QWidget* parent) :
	QWidget(parent),
	columnIndex(columnIndex)
{
	auto layout = new QGridLayout;
	
	layout->addWidget(new QLabel(headerLine, this), 0, 0);
	layout->addWidget(new QLabel(exampleLine, this), 0, 1);

	auto cb = new QComboBox(this);
	QStringList items{"", "date", "amount", "reference"};
	cb->addItems(items);
	layout->addWidget(cb, 0, 2);
	
	setLayout(layout);
}


// date
// senderOwner
// senderIban
// senderBic
// senderOtherId

// receiverOwner
// receiverIban
// receiverBic
// receiverOtherId
// amount
// reference

// id
// senderId
// receiverId
// note
// checked
