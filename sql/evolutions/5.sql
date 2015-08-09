# --- !Ups

create table file (
	id integer primary key autoincrement,
	path text not null,
	formatId integer not null,
	watch integer not null default 1,
	lastImport integer not null default 0,
	lastImportHash text not null default ''
);

# --- !Downs

drop table file;
