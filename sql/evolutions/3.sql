
# --- !Ups

alter table transfer add column internal bool not null default 0;

# --- !Downs

# drop column 'internal'
alter table transfer rename to transferTmp;
create table transfer (
	id integer primary key autoincrement,
	date integer not null,
	fromId integer not null,
	toId integer not null,
	reference text not null,
	amount integer not null,
	note text not null default '',
	checked bool not null default 0
);
insert into transfer (id, date, fromId, toId, reference, amount, note, checked) select id, date, fromId, toId, reference, amount, note, checked from transferTmp;
drop table transferTmp;
