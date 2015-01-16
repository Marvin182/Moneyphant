#ifndef TAG_EDIT_H
#define TAG_EDIT_H

#include <QPlainTextEdit>
#include <QStringList>

class TagEdit : public QPlainTextEdit {
	Q_OBJECT
public:
	TagEdit(QWidget* parent = 0);

public slots:
	void focusOutEvent(QFocusEvent* event);
	void setTags(const QStringList& tags);

signals:
	void tagsAdded(QStringList newTags);
	void tagsRemoved(QStringList removedTags);

private:
	QStringList oldTags;
};

#endif // TAG_EDIT_H