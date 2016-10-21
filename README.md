# Moneyphant

Small programm that helps you combining bank statements from different sources (e.g. many banks allow export in csv files). You can tag, search and sort your transaction and I plan to add some graphs. This helps you to retain control of your expenses. Knowing how much you spend on what (food, car, holidays, ...) lets you safe money more effectively, if you want to ;)

The program doesn't need an internet connection and will not send any data. All your data is saved on your computer.

## Build

### Dependencies
- [Qt](https://www.qt.io/download/) 5.6
- [sqlpp11](https://github.com/rbock/sqlpp11) 0.43
- [sqlpp11-connector-sqlite](https://github.com/rbock/sqlpp11-connector-sqlite3) 0.20
- [libmr](https://github.com/Marvin182/libmr) v0.0.4

Expect libmr the other depencies can be installed using homebrew on MacOS:
```bash
brew install qt5
brew install marvin182/zapfhahn/sqlpp11 --with-sqlite3
```

### Steps
```bash
mkdir build
cd build
cmake ..
make
```

## Roadmap for version 0.2
- Build system
	- [x] Switch to CMake
	- [ ] binaries for Windows
- Tags
	- [ ] auto completion in various places
	- [ ] list of tags
	- [ ] do we need hierarchies?
- Expense overview
	- [x] basic chart
	- [ ] option to view quarters instead of month
	- [ ] show value for each bar
	- [ ] make zooming/ scrolling more user friendly
- DB maintenance
	- [ ] delete unused tags
	- [ ] accounts without transfers
- [ ] IBAN calculation
- [ ] Complete menu
- Export export
	- [ ] filters
	- [ ] transfers
	- [ ] accounts
	- [ ] custom format
- User experience
	- [ ] Upload screenshots
	- [ ] German translation

## Feature Ideas
- Tutorial
- Budgets for tags
- Support for multiple databases switching
- Editing and splitting of transfers
- categories for accounts
- auto tagging based on filters (replacing account tags)
## Devolopement

### Changing the database schema
Moneyphant uses a sqlite3 database. The (sqlite3) database schema is defined in sql/structure.sql After changing it you must create a new evolution in sql/evolutions with update commands for the database. The schema is then managed by the app itself (via the `Evolutions` class). Once released evolutions are not allowed to change, so think twice before changing the database schema.

Steps:

1. add an evolution in sql/evolutions with the correct commands to update an existing database
2. add the evolution file to evolutions.qrc
3. update the structure in sql/structure.sql
4. execute to `sql/ddl2cpp sql/structure.sql src/db db` to update database classes
5. At startup the app will execute evolutions to update the database schema if necessary. You don't need to do anything.

## Thanks To ...
- Gregory Pakosz (Assert Library)
- Roland Bock (sqlpp11 library)
- The awesome Qt developers & community

## License
For know this code will be under the MIT License, but when I have time I will switch to an similar license with a "non-military use only" clause. This code should not be used for any kind of violence.
