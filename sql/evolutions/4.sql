# --- !Ups

create table format (
	id integer primary key autoincrement,
	name varchar(64) default '',
	hashedHeader varchar(256) not null,
	delimiter varchar(8) not null default ";",
	textQualifier varchar(8) not null default "",
	skipFirstLine bool not null default 0,
	columnPositions text not null
);

# --- !Downs

drop table format;
