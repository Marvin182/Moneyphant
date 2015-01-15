#include "TagLabel.h"
#include <QLayout>

TagLabel::TagLabel(const Tag& tag, QWidget* parent) :
	QWidget(parent),
	tag(tag)
{
	auto lay = new QHBoxLayout;

	auto nameLabel = new QLabel(tag.name, this);
	nameLabel->setStyleSheet("padding: 3px; background-color: rgb(255, 255, 255); border-radius: 3px");
	lay->addWidget(nameLabel);

	setLayout(lay);

}

void TagLabel::mousePressEvent(QMouseEvent* event) {
	emit deleteClicked(tag);
}
