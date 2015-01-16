#include "TagEdit.h"
#include <QRegExp>
#include <iostream>
TagEdit::TagEdit(QWidget* parent) : 
	QPlainTextEdit(parent)
{}

void TagEdit::focusOutEvent(QFocusEvent* event) {
	auto tags = document()->toPlainText().split(QRegExp("\\s*[;,]\\s*"));
	
	QStringList newTags;
	for (const auto& t : tags) {
		if (!oldTags.contains(t)) {
			newTags << t;
		}
	}
	if (!newTags.isEmpty()) {
		emit tagsAdded(newTags);
	}

	QStringList removedTags;
	for (const auto& t : oldTags) {
		if (!tags.contains(t)) {
			removedTags << t;
		}
	}
	if (!removedTags.isEmpty()) {
		emit tagsRemoved(removedTags);
	}

	setTags(tags);
}

void TagEdit::setTags(const QStringList& tags) {
	document()->setPlainText(tags.join("; "));
	oldTags = tags;
}
