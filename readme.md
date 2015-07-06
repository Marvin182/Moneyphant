Moneyphant
==========

TODO: short description

Roadmap for Version 0.1
-----------------------
- [x] About Dialog
- [x] Menu
- [ ] CSV Importer
- [ ] Account Search & Filter

Roadmap for Version 0.2
------------------------
- better export (improve statement backups)
- improve tags
	- auto completion in various places
	- list of tags
	- auto delete unused tags (db maintenance?)

Build
-----
TODO: how to build (ddl2cpp)
- db schema changes
	1. update the structure in sql/structure.sql
	2. add an evolution in sql/evolutions with the correct commands to update an existing database
	3. add the evolution file to evolutions.qrc
	4. execute to `sql/ddl2cpp sql/structure.sql src/db db` to update database classes


Thanks To ...
-------------
- Dimitriy Kubyshkin (gitversion.pri)
- Gregory Pakosz (Assert Library)
- the awesome Qt developers & community

