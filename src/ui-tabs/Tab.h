#ifndef TAB_H
#define TAB_H

#include <QWidget>
#include "../sql.h"

class Tab : public QWidget
{
	Q_OBJECT
public:
	Tab(QWidget* parent = 0) :
		QWidget(parent),
		db(nullptr)
	{}

	virtual void init(Db db) { this->db = db; }

public slots:
	virtual void refresh() = 0;
	virtual void focusSearchField() = 0;

protected:
	Db db;
};

#endif // TAB_H
