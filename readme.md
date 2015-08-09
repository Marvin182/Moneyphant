Moneyphant
==========

Small programm that helps you combining bank statements from different sources (e.g. many banks allow export in csv files). You can tag, search and sort your transaction and I plan to add some graphs. This helps you to retain control of your expenses. Knowing how much you spend on what (food, car, holidays, ...) lets you safe money more effectively, if you want to ;)

The program doesn't need an internet connection and will not send any data. All your data is saved on your computer (unencrypted for now).

Roadmap for Version 0.1
-----------------------
- [x] About Dialog
- [x] Menu
- [x] CSV Importer
- [x] Account Search & Filter
- [ ] Watch Statement Files
- [ ] Expenses Overview (needs _main_ tags?)

Roadmap for Version 0.2
------------------------
- better export (improve statement backups)
- improve tags
	- auto completion in various places
	- list of tags
	- auto delete unused tags (db maintenance?)
- Charts for Expenses Overview


Build
-----
### Dependencies
- Qt 5.5
- [sqlpp11](https://github.com/rbock/sqlpp11) 0.34
- [sqlpp11-connector-sqlite](https://github.com/rbock/sqlpp11-connector-sqlite3) 0.15
- [mr-lib](https://github.com/Marvin182/mr) most recent version

### Steps
TODO

Devolopement
------------
### Changing the database schema
Moneyphant uses a sqlite3 database. The (sqlite3) database schema is defined in sql/structure.sql After changing it you must create a new evolution in sql/evolutions with update commands for the database. The schema is then managed by the app itself (via the `Evolutions` class). Once released evolutions are not allowed to change, so think twice before changing the database schema.
Steps:
1. add an evolution in sql/evolutions with the correct commands to update an existing database
2. add the evolution file to evolutions.qrc
3. update the structure in sql/structure.sql
4. execute to `sql/ddl2cpp sql/structure.sql src/db db` to update database classes
5. At startup the app will execute evolutions to update the database schema if necessary. You don't need to do anything.


Thanks To ...
-------------
- Dimitriy Kubyshkin (gitversion.pri)
- Gregory Pakosz (Assert Library)
- the awesome Qt developers & community

