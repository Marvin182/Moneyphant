
# --- !Ups

create table budget (
	id integer primary key autoincrement,
	name varchar(64) not null,
	amount integer not null,
	keepSavings bool not null default 1
);

# --- !Downs

drop table budget;