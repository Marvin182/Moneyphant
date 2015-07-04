#ifndef COLUMN_CHOOSER_H
#define COLUMN_CHOOSER_H

#include "mr/common.h"
#include <QMap>
#include <QStringList>
#include <QWidget>
#include <QComboBox>
#include <QLabel>


class ColumnChooser : public QObject {
	Q_OBJECT
public:
	typedef QMap<QString, int> InputFormat;

	ColumnChooser(int columnIndex, cqstring headerLine, cqstring exampleLine, QWidget* parent);
	~ColumnChooser();

	int columnIndex() const { return _columnIndex; }
	int inputTypeIndex() const { return cbInputType->currentIndex(); }
	cqstring inputType() const { return inputTypes[inputTypeIndex()]; }

	void unsetIfInputTypeIndex(int inputTypeIndex);

	void load(const InputFormat& format);
	void save(InputFormat& format) const;

signals:
	void inputTypeIndexChanged(int columnIndex, int inputTypeIndex);

private slots:
	void onCurrentInputTypeChanged(int inputTypeIndex);

private:
	static QStringList inputTypes;

	const int _columnIndex;

	QLabel* lHeaderLabel;
	QLabel* lExampleLabel;
	QComboBox* cbInputType;
};

#endif // COLUMN_CHOOSER_H
