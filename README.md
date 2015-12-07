Moneyphant
==========

Small programm that helps you combining bank statements from different sources (e.g. many banks allow export in csv files). You can tag, search and sort your transaction and I plan to add some graphs. This helps you to retain control of your expenses. Knowing how much you spend on what (food, car, holidays, ...) lets you safe money more effectively, if you want to ;)

The program doesn't need an internet connection and will not send any data. All your data is saved on your computer (unencrypted for now).

Roadmap for version 0.2
-----------------------
- [ ] Improved tags
	- [ ] Auto completion in various places
	- [ ] List of tags (incl. number of associated transfers)
	- [ ] Drop account tags
- [ ] Increase user base
	- [ ] Binaries for Windows
	- [ ] Upload screenshots
	- [ ] German translation
- [ ] Expense overview with bar charts
	- [x] Basic chart
	- [ ] Option to view quarters instead of month
	- [ ] Make zooming and scrolling more user friendly
	- [ ] Show value on bar
- [ ] Database maintenance
	- [ ] delete unused tags
- [ ] IBAN calculation
- [ ] Special Account for total Balance of User
- [ ] Auto tagging based on filters
- [ ] Performance improvements
	- [ ] only scan a statement file if it could have changed (e.g. checked date modified, file size, hash)

Feature Ideas
-------------
- Usability
	- Add tooltips
	- Complete menu
- Documentation
	- Base steps and ideas
	- Shortcuts
	- Transfer Filtering
	- Better license
- Export (using filtering)
	- transfers (all/filtered/selected), accounts
	- custom format
	- automated (as backup)
- Transfers
	- Remember source of transfer
	- Edit transfer
	- Add transfers manually (sync to special file)
	- Find unmatched internal transfers
- Accounts
	- Categories (e.g.: own, bank, friends)
	- Types (e.g. bank account, credit card, cash, PayPal)
	- Find duplicates
- Switching database
Add no-military-use clause to license

Build
-----
### Dependencies
- Qt 5.5
- [sqlpp11](https://github.com/rbock/sqlpp11) 0.34
- [sqlpp11-connector-sqlite](https://github.com/rbock/sqlpp11-connector-sqlite3) 0.15
- [mr](https://github.com/Marvin182/mr) 0.0.3

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