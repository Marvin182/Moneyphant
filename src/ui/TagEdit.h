#ifndef TAG_EDIT_H
#define TAG_EDIT_H

#include <vector>
#include <QPlainTextEdit>
#include <QStringList>

class TagEdit : public QPlainTextEdit {
	Q_OBJECT
public:
	TagEdit(QWidget* parent = 0);

	std::pair<QStringList, QStringList> tagChanges();

public slots:
	void focusOutEvent(QFocusEvent* event);
	void setTags(const QStringList& tags, const std::vector<int>& accountIds);

signals:
	void tagsAdded(QStringList newTags, const std::vector<int>& accountIds);
	void tagsRemoved(QStringList removedTags, const std::vector<int>& accountIds);

private:
	QStringList oldTags;
	std::vector<int> accountIds;
};

#endif // TAG_EDIT_H