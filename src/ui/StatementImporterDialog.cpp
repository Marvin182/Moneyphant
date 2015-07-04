#include "StatementImporterDialog.h"
#include "ui_StatementImporterDialog.h"

#include <vector>
#include <algorithm>
#include <QFileInfo>
#include <QStringRef>
#include <QGridLayout>

const std::vector<QString> delimiters{",", ";", "\t"};
const std::vector<QString> textQualifiers{"\"", "\'"};

StatementImporterDialog::StatementImporterDialog(Db db, cqstring filePath, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StatementImporterDialog),
	_success(false),
	db(db)
{
	ui->setupUi(this);

	readFile(filePath);

	ui->delimiter->setCurrentIndex(guessDelimiter());
	ui->textQualifier->setCurrentIndex(guessTextQualifier());
	assert_error(ui->delimiter->currentIndex() >= 0, "guessed invalid delimiter index %d", ui->delimiter->currentIndex());
	assert_error(ui->textQualifier->currentIndex() >= 0, "guessed invalid text qualifier index %d", ui->textQualifier->currentIndex());
	connect(ui->delimiter, SIGNAL(currentIndexChanged(int)), this, SLOT(createColumnChoosers()));
	connect(ui->textQualifier, SIGNAL(currentIndexChanged(int)), this, SLOT(createColumnChoosers()));

	createColumnChoosers();

	connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui->importStatements, SIGNAL(clicked()), this, SLOT(doImport()));
}

StatementImporterDialog::~StatementImporterDialog() {
	delete ui;
}

const QString& StatementImporterDialog::delimiter() const {
	return delimiters[ui->delimiter->currentIndex()];
}

const QString& StatementImporterDialog::textQualifier() const {
	return textQualifiers[ui->textQualifier->currentIndex()];
}

ColumnChooser::InputFormat StatementImporterDialog::columnsOrder() const {
	ColumnChooser::InputFormat format;
	for (auto ic : columnChoosers) {
		ic->save(format);
	}
	return format;
}

bool StatementImporterDialog::validateFormat() {
	auto format = columnsOrder();
	ui->importStatements->setEnabled(false);

	if (!format.contains("date")) {
		return false;
	}
	if (!format.contains("amount")) {
		return false;
	}
	if (!format.contains("receiverIban") && !format.contains("receiverId") && !format.contains("receiverEmail")) {
		return false;
	}

	ui->importStatements->setEnabled(true);
	return true;
}

void StatementImporterDialog::doImport() {
	assert_error(validateFormat());
	_success = true;
}

void StatementImporterDialog::onColumnChooserChanged(int columnIndex, int inputTypeIndex) {
	if (inputTypeIndex > 0) {
		for (auto ic : columnChoosers) {
			if (ic->columnIndex() != columnIndex) {
				ic->unsetIfInputTypeIndex(inputTypeIndex);
			}
		}
	}
	validateFormat();
}

void StatementImporterDialog::readFile(cqstring filePath) {
	QFile file(filePath);

	// verify that the file is opened correctly
	assert_error(file.exists(), "file '%s' does not exists", cstr(filePath));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		assert_error(false, "Could not open statement file '%s'", cstr(filePath));
	}
	assert_error(!file.atEnd(), "statement file '%s' is empty", cstr(filePath));

	// read all lines
	lines.clear();
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

	ui->filename->setText(QFileInfo(filePath).fileName());

	auto firstLines = lines.front();
	for (int i = 1; i < lines.size() && i < 10; i++) {
		firstLines += "\n" + lines[i];
	}
	ui->firstLines->setPlainText(firstLines);
}

int StatementImporterDialog::guessDelimiter() {
	// find best delimiter by choosing the one the appears most in the first line
	std::vector<int> delimiterCounts;
	for (const auto& delim : delimiters) {
		delimiterCounts.push_back(lines.front().count(delim));
	}
	auto it_max = std::max_element(delimiterCounts.begin(), delimiterCounts.end());
	assert_error(*it_max > 0, "no delimiter found in '%s'", cstr(lines.front()));
	int index = std::distance(delimiterCounts.begin(), it_max);

	assert_error(index < ui->delimiter->count(), "index was %d, item count was %d", index, ui->delimiter->count());

	return index;
}

int StatementImporterDialog::guessTextQualifier() {
	const auto& delim = delimiters[ui->delimiter->currentIndex()];
	auto header = lines.front().split(delim, QString::SkipEmptyParts);

	// look for a matching text qualifier
	for (int i = 0; i < textQualifiers.size(); i++) {
		const  auto& tq = textQualifiers[i];
		for (int j = 0; j < header.size(); j++) {
			assert_error(!header[j].isEmpty(), "splitted line '%s' using delimiter '%s' shouldn't contain empty parts anymore", cstr(lines.front()), cstr(delim));
			if (!header[j].startsWith(tq) || !header[j].endsWith(tq)) {
				break;
			}
			if (j == header.size() - 1) {
				return i;
			}
		}
	}

	return textQualifiers.size(); // index for none
}

void StatementImporterDialog::createColumnChoosers() {
	auto header = lines.front().split(delimiter());
	if (ui->textQualifier->currentIndex() < textQualifiers.size()) {
		for (auto& h : header) {
			if (!h.isEmpty() && h.length() > 1) {
				h = QStringRef(&h, 1, h.length() - 2).toString();
			}
		}
	}

	bool hasExampleLine = lines.size() > 1;
	auto exampleLine = hasExampleLine ? lines[1].split(delimiter()) : QStringList();
	if (ui->textQualifier->currentIndex() < textQualifiers.size()) {
		for (auto& l : exampleLine) {
			if (!l.isEmpty() && l.length() > 1) {
				l = QStringRef(&l, 1, l.length() - 2).toString();
			}
		}
	}

	for (auto dc : columnChoosers) { delete dc; }
	columnChoosers.clear();
	delete ui->columnChoosers->layout();
	new QGridLayout(ui->columnChoosers);

	for (int i = 0; i < header.size(); i++) {
		columnChoosers.push_back(new ColumnChooser(i, header[i], hasExampleLine ? exampleLine[i] : "", ui->columnChoosers));
		connect(columnChoosers.back(), SIGNAL(inputTypeIndexChanged(int, int)), this, SLOT(onColumnChooserChanged(int, int)));
	}

	validateFormat();
}
