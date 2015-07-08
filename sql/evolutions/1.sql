
# --- !Ups

create table transfer (
	id integer primary key autoincrement,
	date integer not null,
	fromId integer not null,
	toId integer not null,
	reference text not null,
	amount integer not null,
	note text not null default '',
	checked integer not null default 0
);

# --- !Downs

drop table transfer;