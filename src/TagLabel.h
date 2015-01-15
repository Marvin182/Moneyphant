#ifndef TAG_LABEL_H
#define TAG_LABEL_H

#include "Global.h"
#include "Tag.h"
#include <QLabel>
#include <QMouseEvent>

class TagLabel : public QWidget {
	Q_OBJECT
public:
	Tag tag;

	TagLabel(const Tag& tag, QWidget* parent = 0);

public slots:
	void mousePressEvent(QMouseEvent* event);

signals:
	void deleteClicked(Tag tag);
};

#endif // TAG_LABEL_H
