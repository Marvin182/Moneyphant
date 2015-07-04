# --- !Ups

create table format (
	id integer primary key autoincrement,
	name varchar(64) default '',
	delimiter varchar(8),
	textQualifier varchar(8),
	columnsOrder text
);

# --- !Downs

drop table format;