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
- [x] Watch Statement Files
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
- [mr](https://github.com/Marvin182/mr) v0.0.2

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

License
-------

The MIT License (MIT)

Copyright (c) 2015 Marvin Ritter

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.