#include "TagEdit.h"

#include <QRegExp>
#include "../TagHelper.h"

TagEdit::TagEdit(QWidget* parent) : 
	QPlainTextEdit(parent),
	oldTags(),
	accountIds()
{}

std::pair<QStringList, QStringList> TagEdit::tagChanges() {
	auto tags = TagHelper::splitTags(document()->toPlainText());
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
	for (const auto& t : TagHelper::splitTags(document()->toPlainText())) {
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
	document()->setPlainText(tags.join(" "));
}

void TagEdit::setTags(const QStringList& tags, const std::vector<int>& accountIds) {
	focusOutEvent(nullptr);
	this->accountIds = accountIds;

	oldTags = tags;
	document()->setPlainText(tags.join(" "));
}
