
# --- !Ups

create table account (
	id integer primary key autoincrement,
	isOwn integer not null default 0,
	name text not null default '',
	owner text not null,
	iban text not null default '',
	bic text not null default '',
	accountNumber text not null default '',
	bankCode text not null default ''
);

# --- !Downs

drop table account;