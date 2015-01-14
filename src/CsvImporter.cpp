#include "CsvImporter.h"

CsvImporter::CsvImporter(Db db) :
	db(db)
{}

void CsvImporter::importMissingTransfers(string path) {
	QFile file(path);
	assert(file.exists() && "founded statement file dosen't exists.");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		assert(false && "Could not open statement file");
	}


	
	std::vector<Operation> ops;
	bool firstRow = true;
	assert(!file.atEnd() && "file is empty");
	while (!file.atEnd()) {
		auto fields = QString(file.readLine()).split(';');
		assert(fields.size() > 8);
		for (auto& f : fields) {
			f = f.trimmed();
			if (f.length() > 1) {
				assert(f[0] == '"' && f[f.length() - 1] == '"');
				f = QStringRef(&f, 1, f.length() - 2).toString();
			}
		}
		if (firstRow) {
			firstRow = false;
			continue;
		}

		auto& from = accounts->addOrGet(Account("", fields[0], ""));
		auto& to = accounts->addOrGet(Account(fields[5], fields[6], fields[7]));
		assert(from.id >= 0 && from.id < 100000 && to.id >= 0 && to.id < 1000000);
		assert(from.id >= 0 && to.id >= 0 && from.id != to.id);
		Operation op(fields[1], from, to, fields[4], fields[8]);
		if (!contains(op)) {
			ops.push_back(op);
		}
	}
	insert(ops);
}