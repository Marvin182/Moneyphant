#ifndef DATA_CHOOSER_H
#define DATA_CHOOSER_H

#include "../globals/all.h"
#include <vector>
#include <QWidget>
#include <QComboBox>
#include <QLabel>

class DataChooser : public QObject {
	Q_OBJECT
public:
	DataChooser(int row, cqstring headerLine, cqstring exampleLine, QWidget* parent);
	~DataChooser();

	int row() const { return _row; }
	void unsetIndex(int index);

	void load(const std::vector<int>& format);
	void save(std::vector<int>& format);

signals:
	void columnChanged(int row, int index);

private slots:
	void onCurrentIndexChanged(int index);

private:
	const int _row;

	QLabel* headerLabel;
	QLabel* exampleLabel;
	QComboBox* comboBox;
};

#endif // DATA_CHOOSER_H
