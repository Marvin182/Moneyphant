Moneyphant
==========

TODO: short description

Roadmap for Version 0.1
-----------------------
- [x] About Dialog
- [x] Menu
- [x] CSV Importer
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

Devolopement
------------
The (sqlite3) database schema is defined in sql/structure.sql After changing it you must create a new evolution in sql/evolutions with update commands for the database. The schema is then managed by the app itself (via the Evolutions class). Once released Evolutions are not allowed to change.
Steps:
1. update the structure in sql/structure.sql
2. add an evolution in sql/evolutions with the correct commands to update an existing database
3. add the evolution file to evolutions.qrc
4. execute to `sql/ddl2cpp sql/structure.sql src/db db` to update database classes
5. (Nothing to do for you) At startup the app will execute evolutions to update the database schema if necessary.


Thanks To ...
-------------
- Dimitriy Kubyshkin (gitversion.pri)
- Gregory Pakosz (Assert Library)
- the awesome Qt developers & community

