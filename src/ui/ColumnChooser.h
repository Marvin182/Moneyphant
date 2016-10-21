#ifndef COLUMN_CHOOSER_H
#define COLUMN_CHOOSER_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <mr/common>
#include <mr/qt>

class ColumnChooser : public QObject {
	Q_OBJECT
public:
	ColumnChooser(int columnIndex, cqstring headerLine, cqstring exampleLine, QWidget* parent);
	~ColumnChooser();

	int columnIndex() const { return _columnIndex; }
	cqstring inputType() const;

	void set(cqstring headerLine, cqstring exampleLine);
	void setInputType(cqstring inputType);
	void unset();
	void unsetIfInputTypeIndex(cqstring inputType);

signals:
	void inputTypeChanged(int columnIndex, const QString& inputType);

private:
	const int _columnIndex;

	QLabel* lHeaderLabel;
	QLabel* lExampleLabel;
	QComboBox* cbInputType;

	int inputTypeIndex() const { return cbInputType->currentIndex(); }
};

#endif // COLUMN_CHOOSER_H
