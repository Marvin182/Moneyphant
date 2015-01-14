
# --- !Ups

create table account (
	id integer primary key autoincrement,
	isOwn bool not null default 0,
	name varchar(128) not null default '',
	owner varchar(128) not null,
	iban varchar(64) not null default '',
	bic varchar(32) not null default '',
	accountNumber varchar(32) not null default '',
	bankCode varchar(32) not null default ''
);

# --- !Downs

drop table account;