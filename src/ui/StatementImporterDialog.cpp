#include "StatementImporterDialog.h"
#include "ui_StatementImporterDialog.h"

#include "DataChooser.h"
#include <vector>
#include <algorithm>
#include <QStringRef>

StatementImporterDialog::StatementImporterDialog(cqstring filePath, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StatementImporterDialog),
	file(filePath)
{
	ui->setupUi(this);

	// verify that the file is opened correctly
	assert_error(file.exists(), "file '%s' does not exists", cstr(filePath));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		assert_error(false, "Could not open statement file '%s'", cstr(filePath));
	}
	assert_error(!file.atEnd(), "statement file '%s' is empty", cstr(filePath));

	// read all lines
	std::vector<QString> lines;
	while (!file.atEnd()) {
		auto line = file.readLine();
		if (line.isEmpty()) {
			continue;
		}
		assert_debug(line[line.length() - 1] == '\n');
		lines.push_back(line.left(line.length() - 1));
	}
	assert_error(file.atEnd(), "statement file not read until the end");
	assert_error(!lines.empty(), "statement file had only empty lines");

	// find best delimiter by choosing the one the appears most in the first line
	std::vector<QString> delimiter{",", ";", "\t"};
	std::vector<int> delimiterCount;
	for (const auto& delim : delimiter) {
		delimiterCount.push_back(lines.front().count(delim));
	}
	int bestDelimiterIndex = std::distance(delimiterCount.begin(), std::max_element(delimiterCount.begin(), delimiterCount.end()));

	// look for a matching text qualifier
	std::vector<QString> textQualifier{"\"", "\'"};
	auto header = lines.front().split(delimiter[bestDelimiterIndex], QString::SkipEmptyParts);
	int bestTextQualifierIndex = textQualifier.size();
	for (int i = 0; i < textQualifier.size(); i++) {
		const  auto& tq = textQualifier[i];
		for (int j = 0; j < header.size(); j++) {
			assert_error(!header[j].isEmpty(), "splitted line '%s' using delimiter '%s' shouldn't contain empty parts anymore", cstr(lines.front()), cstr(delimiter[bestDelimiterIndex]));
			if (!header[j].startsWith(tq) || !header[j].endsWith(tq)) {
				break;
			}
			if (j == header.size() - 1) {
				bestTextQualifierIndex = i;
			}
		}
	}

	ui->filename->setText(file.fileName());
	ui->delimiter->setCurrentIndex(bestDelimiterIndex);
	ui->textQualifier->setCurrentIndex(bestTextQualifierIndex);

	bool hasExampleLine = lines.size() > 1;
	auto exampleLine = hasExampleLine ? lines[1].split(delimiter[bestDelimiterIndex]) : QStringList();
	for (int i = 0; i < header.size(); i++) {
		auto w = new DataChooser(i, header[i], hasExampleLine ? exampleLine[i] : "", this);
		ui->dataChoosers->layout()->addWidget(w);
	}

	auto firstLines = lines.front();
	for (int i = 1; i < lines.size() && i < 10; i++) {
		firstLines += "\n" + lines[i];
	}
	ui->firstLines->setPlainText(firstLines);
}

StatementImporterDialog::~StatementImporterDialog()
{
	delete ui;
}
