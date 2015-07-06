#include "StatementImporterDialog.h"
#include "ui_StatementImporterDialog.h"

#include <algorithm>
#include <QFile>
#include <QFileInfo>
#include <QStringRef>
#include <QGridLayout>
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

	lines = readFile(filePath);

	_format.setHeader(lines.front());

	if (!_format.load(db)) {
		_format.delimiter = guessDelimiter();
		_format.textQualifier = guessTextQualifier();
		assert_error(delimiterIndex(_format.delimiter) >= 0, "guessed invalid delimiter (%s, %d)", cstr(_format.delimiter), delimiterIndex(_format.delimiter));
		assert_error(textQualifierIndex(_format.textQualifier) >= 0, "guessed invalid text qualifier (%s, %d)", cstr(_format.textQualifier), textQualifierIndex(_format.textQualifier));
	}

	ui->delimiter->setCurrentIndex(delimiterIndex(_format.delimiter));
	ui->textQualifier->setCurrentIndex(textQualifierIndex(_format.textQualifier));
	ui->skipFirstLine->setEnabled(_format.skipFirstLine);

	connect(ui->delimiter, SIGNAL(currentIndexChanged(int)), this, SLOT(onDelimiterChanged(int)));
	connect(ui->textQualifier, SIGNAL(currentIndexChanged(int)), this, SLOT(onTextQualifierChanged(int)));
	connect(ui->skipFirstLine, SIGNAL(stateChanged(int)), this, SLOT(onSkipFirstLineChanged(int)));

	createColumnChoosers();
	for (auto it = _format.columnPositions.begin(); it != _format.columnPositions.end(); it++) {
		assert_error(it.value() >= 0 && it.value() < columnChoosers.size());
		columnChoosers[it.value()]->setInputType(it.key());
	}

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
	// remove old columns choosers
	for (auto dc : columnChoosers) { delete dc; }
	columnChoosers.clear();
	delete ui->columnChoosers->layout();
	new QGridLayout(ui->columnChoosers);

	auto header = lineAsStringList(lines.front());
	bool hasExampleLine = lines.size() > 1;
	auto exampleLine = hasExampleLine ? lineAsStringList(lines[1]) : QStringList();

	for (int i = 0; i < header.size(); i++) {
		columnChoosers.push_back(new ColumnChooser(i, header[i], hasExampleLine ? exampleLine[i] : "", ui->columnChoosers));
		connect(columnChoosers.back(), SIGNAL(inputTypeChanged(int, const QString&)), this, SLOT(onColumnChooserChanged(int, const QString&)));
	}
}

void StatementImporterDialog::onColumnChooserChanged(int columnIndex, const QString& inputType) {
	auto& cp = _format.columnPositions;

	// remove old column meaning
	for (auto it = cp.begin(); it != cp.end(); it++) {
		if (it.value() == columnIndex) {
			it = cp.erase(it);
			if (it == cp.end()) {
				break;
			}
		}
		if (it.key() == inputType) {
			assert_error(it.value() >= 0 && it.value() < columnChoosers.size(), "it.value() was %d, columnChoosers.size() was %lu", it.value(), columnChoosers.size());
			auto ic = columnChoosers[it.value()];
			assert_error(ic->inputType() == inputType);
			ic->unset();
		}
	}

	if (!inputType.isEmpty()) {
		cp[inputType] = columnIndex;
	}

	validateFormat(_format);
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

void StatementImporterDialog::saveFormatToDb() {
	//saveFormatToDb(_format);
}

bool StatementImporterDialog::validateFormat(const StatementFileFormat& format) {
	ui->importStatements->setEnabled(false);
	auto& cp = format.columnPositions;

	if (!cp.contains("date")) {
		return false;
	}
	if (!cp.contains("amount")) {
		return false;
	}
	if (!cp.contains("receiverIban") && !cp.contains("receiverId") && !cp.contains("receiverEmail")) {
		return false;
	}

	ui->importStatements->setEnabled(true);
	return true;
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

std::vector<QString> StatementImporterDialog::readFile(cqstring filePath) {
	QFile file(filePath);

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

	ui->filename->setText(QFileInfo(filePath).fileName());

	auto exampleLines = lines.front();
	for (int i = 1; i < std::min(5ul, lines.size()); i++) {
		exampleLines += "\n" + lines[i];
	}
	ui->exampleLines->setPlainText(exampleLines);

	return lines;
}

QStringList StatementImporterDialog::lineAsStringList(cqstring line) const {
	auto list = line.split(delimiter());

	// remove text qualifiers
	if (hasTextQualifier()) {
		int tqLen = textQualifier().length();
		for (auto& w : list) {
			if (!w.isEmpty() && w.length() >= 2 * tqLen) {
				w = QStringRef(&w, tqLen, w.length() - 2 * tqLen).toString();
			}
		}
	}

	return list;
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