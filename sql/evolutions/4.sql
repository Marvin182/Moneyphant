# --- !Ups

create table format (
	id integer primary key autoincrement,
	name text default '',
	hashedHeader text not null,
	delimiter text not null default ";",
	textQualifier text not null default "",
	skipFirstLine integer not null default 0,
	dateFormat text not null default "dd.MM.yy",
	columnPositions text not null
);

# --- !Downs

drop table format;
