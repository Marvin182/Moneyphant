#include "TagEdit.h"

#include <QRegExp>

TagEdit::TagEdit(QWidget* parent) : 
	QPlainTextEdit(parent),
	oldTags(),
	accountIds()
{}

std::pair<QStringList, QStringList> TagEdit::tagChanges() {
	auto tags = document()->toPlainText().split(QRegExp("\\s*[;,]\\s*"));
	auto changes = std::make_pair(QStringList(), QStringList());
	return changes;
	for (const auto& t : tags) {
		if (!oldTags.contains(t)) {
			// added
			changes.first << t;
		}
	}

	for (const auto& t : oldTags) {
		if (!tags.contains(t)) {
			// removed
			changes.second << t;
		}
	}

	return changes;
}

void TagEdit::focusOutEvent(QFocusEvent* event) {
	QStringList tags;
	for (const auto& t : document()->toPlainText().split(QRegExp("[;,]"))) {
		auto tag = t.trimmed();
		if (!tag.isEmpty()) {
			tags << tag;
		}
	}

	QStringList newTags;
	for (const auto& t : tags) {
		if (!oldTags.contains(t)) {
			newTags << t;
		}
	}
	if (!newTags.isEmpty()) {
		emit tagsAdded(newTags, accountIds);
	}

	QStringList removedTags;
	for (const auto& t : oldTags) {
		if (!tags.contains(t)) {
			removedTags << t;
		}
	}
	if (!removedTags.isEmpty()) {
		emit tagsRemoved(removedTags, accountIds);
	}

	oldTags = tags;
	document()->setPlainText(tags.join("; "));
}

void TagEdit::setTags(const QStringList& tags, const std::vector<int>& accountIds) {
	focusOutEvent(nullptr);
	this->accountIds = accountIds;

	oldTags = tags;
	document()->setPlainText(tags.join("; "));
}
