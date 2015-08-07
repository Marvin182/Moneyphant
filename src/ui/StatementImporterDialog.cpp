#include "StatementImporterDialog.h"
#include "ui_StatementImporterDialog.h"

#include <algorithm>
#include <QFile>
#include <QFileInfo>
#include <QStringRef>
#include <QGridLayout>
#include <QTextStream>
#include <QTextCodec>
#include <QCryptographicHash>

const std::vector<QString> Delimiters{",", ";", "\t"};
const std::vector<QString> TextQualifiers{"\"", "\'", ""};

StatementImporterDialog::StatementImporterDialog(Db db, cqstring filePath, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StatementImporterDialog),
	_success(false),
	_format(),
	db(db)
{
	ui->setupUi(this);

	// file is only read once
	lines = readFile(filePath);
	assert_error(!lines.empty());

	// try loading format from database (identify formats based on a hash of the header)
	_format.setHeader(lines.front());
	if (!_format.load(db)) {
		// format not found in the database, guess good starting values
		_format.delimiter = guessDelimiter();
		_format.textQualifier = guessTextQualifier();
		assert_error(delimiterIndex(_format.delimiter) >= 0, "guessed invalid delimiter (%s, %d)", cstr(_format.delimiter), delimiterIndex(_format.delimiter));
		assert_error(textQualifierIndex(_format.textQualifier) >= 0, "guessed invalid text qualifier (%s, %d)", cstr(_format.textQualifier), textQualifierIndex(_format.textQualifier));
	}

	// sync ui with format
	ui->delimiter->setCurrentIndex(delimiterIndex(_format.delimiter));
	ui->textQualifier->setCurrentIndex(textQualifierIndex(_format.textQualifier));
	ui->skipFirstLine->setEnabled(_format.skipFirstLine);
	ui->dateFormat->setText(_format.dateFormat);
	ui->lineSuffix->setText(_format.lineSuffix);
	if (!_format.lineSuffix.isEmpty()) {
		for (auto& line : lines) {
			line += _format.lineSuffix;
		}
	}

	// create and load ColumnChoosers
	createColumnChoosers();
	for (auto it = _format.columnPositions.begin(); it != _format.columnPositions.end(); it++) {
		assert_error(it.value() >= 0 && it.value() < columnChoosers.size());
		columnChoosers[it.value()]->setInputType(it.key());
	}

	// listen for changes to format options
	connect(ui->delimiter, SIGNAL(currentIndexChanged(int)), SLOT(onDelimiterChanged(int)));
	connect(ui->textQualifier, SIGNAL(currentIndexChanged(int)), SLOT(onTextQualifierChanged(int)));
	connect(ui->skipFirstLine, SIGNAL(stateChanged(int)), SLOT(onSkipFirstLineChanged(int)));
	connect(ui->dateFormat, &QLineEdit::textEdited, [&](cqstring text) { _format.dateFormat = text; });
	connect(ui->lineSuffix, SIGNAL(textEdited(const QString&)), SLOT(onLineSuffixChanged(const QString&)));

	connect(ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->importStatements, &QPushButton::clicked, [&]() {
		_format.save(db);
		accept();
	});
}

StatementImporterDialog::~StatementImporterDialog() {
	delete ui;
}


//
// Private Slots
//

void StatementImporterDialog::createColumnChoosers() {
	auto header = lineAsStringList(lines.front());
	bool hasExampleLine = lines.size() > 1;
	auto exampleLine = hasExampleLine ? lineAsStringList(lines[1]) : QStringList();

	// create a ColumnChooser for each header field (= column)
	for (int i = 0; i < header.size(); i++) {
		if (i < columnChoosers.size()) {
			// reuse existing ColumnChooser
			columnChoosers[i]->set(header[i], hasExampleLine ? exampleLine[i] : "");
		} else {
			columnChoosers.push_back(new ColumnChooser(i, header[i], hasExampleLine ? exampleLine[i] : "", ui->columnChoosers));
			connect(columnChoosers.back(), SIGNAL(inputTypeChanged(int, const QString&)), this, SLOT(onColumnChooserChanged(int, const QString&)));
		}
	}

	// drop unused column choosers
	// from layout...
	while (columnChoosers.size() > header.size()) {
		delete columnChoosers.back();
		columnChoosers.pop_back();
	}
	// and from format...
	auto& cp = _format.columnPositions;
	for (auto it = cp.begin(); it != cp.end(); it++) {
		if (it.value() >= header.size()) {
			it = cp.erase(it);
		}
	}

	ui->importStatements->setEnabled(_format.isValid());
}

void StatementImporterDialog::onColumnChooserChanged(int columnIndex, const QString& inputType) {
	auto& cp = _format.columnPositions;

	// unset other ColumnChooser pointing to the same inputType
	if (cp.contains(inputType)) {
		auto ic = columnChoosers[cp[inputType]];
		assert_error(ic->inputType() == inputType);
		ic->unset();
	}

	// erase old association between previous inputType and the column
	for (auto it = cp.begin(); it != cp.end(); it++) {
		if (it.value() == columnIndex) {
			cp.erase(it);
			break;
		}
	}

	// set new association between inputType and column
	if (!inputType.isEmpty()) {
		cp[inputType] = columnIndex;
	}

	ui->importStatements->setEnabled(_format.isValid());
}

void StatementImporterDialog::onDelimiterChanged(int index) {
	_format.delimiter = Delimiters[index];
	createColumnChoosers();
}

void StatementImporterDialog::onTextQualifierChanged(int index) {
	_format.textQualifier = TextQualifiers[index];
	createColumnChoosers();
}

void StatementImporterDialog::onSkipFirstLineChanged(int state) {
	_format.skipFirstLine = state == Qt::Checked; 
}

void StatementImporterDialog::onLineSuffixChanged(const QString& newSuffix) {
	auto oldSuffix = _format.lineSuffix;
	_format.lineSuffix = newSuffix;

	for (auto& line : lines) {
		if (!oldSuffix.isEmpty()) {
			line = line.left(line.length() - oldSuffix.length());
		}
		line += newSuffix;
	}

	createColumnChoosers();
}

// 
// Private Methods
//

bool StatementImporterDialog::hasTextQualifier() const {
	return _format.textQualifier != "";
}

int StatementImporterDialog::delimiterIndex(cqstring del) const {
	auto it = std::find(Delimiters.begin(), Delimiters.end(), del);
	assert_debug(it != Delimiters.end());
	return it == Delimiters.end() ? -1 : std::distance(Delimiters.begin(), it);
}

int StatementImporterDialog::textQualifierIndex(cqstring tq) const {
	auto it = std::find(TextQualifiers.begin(), TextQualifiers.end(), tq);
	return it == TextQualifiers.end() ? -1 : std::distance(TextQualifiers.begin(), it);
}

const QString& StatementImporterDialog::delimiter() const {
	return _format.delimiter;
}

const QString& StatementImporterDialog::textQualifier() const {
	return _format.textQualifier;
}

std::vector<QString> StatementImporterDialog::readFile(cqstring filename) {
	std::vector<QString> lines;
	
	mr::io::readTextFile(filename, [&](QTextStream& in) {
		// read all lines
		while (!in.atEnd()) {
			auto line = in.readLine().trimmed();
			if (line.isEmpty()) {
				continue;
			}
			assert_debug(!line.endsWith('\n'), "line should not end with new line char: '%s'", cstr(line));
			lines.push_back(line + _format.lineSuffix);
		}
		assert_error(in.atEnd(), "statement file not read until the end");
		assert_error(!lines.empty(), "statement file had only empty lines");
	});
	
	ui->filename->setText(QFileInfo(filename).fileName());

	auto exampleLines = lines.front();
	for (int i = 1; i < std::min(5ul, lines.size()); i++) {
		exampleLines += "\n" + lines[i];
	}
	ui->exampleLines->setPlainText(exampleLines);

	return lines;
}

QStringList StatementImporterDialog::lineAsStringList(cqstring line) const {
	return mr::string::splitAndTrim(line, _format.delimiter, _format.textQualifier);	
}

QString StatementImporterDialog::guessDelimiter() {
	// find best delimiter by choosing the one the appears most in the first line
	std::vector<int> delimiterCounts;
	for (const auto& del : Delimiters) {
		delimiterCounts.push_back(lines.front().count(del));
	}
	auto it_max = std::max_element(delimiterCounts.begin(), delimiterCounts.end());
	assert_error(*it_max > 0, "no delimiter found in '%s'", cstr(lines.front()));
	int index = std::distance(delimiterCounts.begin(), it_max);

	assert_error(index < ui->delimiter->count(), "index was %d, item count was %d", index, ui->delimiter->count());

	return Delimiters[index];
}

QString StatementImporterDialog::guessTextQualifier() {
	auto header = lines.front().split(delimiter(), QString::SkipEmptyParts);

	// look for a matching text qualifier
	// tq can be any length, not just 1 char
	for (auto tq : TextQualifiers) {
		if (tq == "") continue;
		for (int j = 0; j < header.size(); j++) {
			assert_debug(!header[j].isEmpty(), "splitted line '%s' using delimiter '%s' shouldn't contain empty parts anymore", cstr(lines.front()), cstr(delimiter()));
			if (!header[j].startsWith(tq) || !header[j].endsWith(tq)) {
				break;
			} else if (j == header.size() - 1) {
				// reached line end and tq worked for all fields
				return tq;
			}
		}
	}

	return "";
}
