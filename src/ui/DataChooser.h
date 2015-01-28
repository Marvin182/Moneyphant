#ifndef DATA_CHOOSER_H
#define DATA_CHOOSER_H

#include "../globals/all.h"
#include <QWidget>

class DataChooser : public QWidget {
	Q_OBJECT
public:
	DataChooser(int columnIndex, cqstring headerLine, cqstring exampleLine, QWidget* parent = 0);

public slots:

signals:

private:
	int columnIndex;
};

#endif // DATA_CHOOSER_H
