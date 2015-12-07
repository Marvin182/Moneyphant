# --- !Ups

# add start balance for accounts
alter table account add column currency text not null default 'EUR';
alter table transfer add column currency text not null default 'EUR';

# --- !Downs

# drop column currency
alter table account rename to tmp_account;
create table account (
	id integer primary key autoincrement,
	isOwn integer not null default 0,
	name text not null default '',
	owner text not null,
	iban text not null default '',
	bic text not null default '',
	accountNumber text not null default '',
	bankCode text not null default '',
	initialBalance integer not null default 0
);
insert into account (id, isOwn, name, owner, iban, bic, accountNumber, bankCode, initialBalance) select id, isOwn, name, owner, iban, bic, accountNumber, bankCode, initialBalance from tmp_account;
drop table tmp_account;

# drop column currency 
alter table transfer rename to tmp_transfer;
create table transfer (
	id integer primary key autoincrement,
	date integer not null,
	fromId integer not null,
	toId integer not null,	
	reference text not null,
	amount integer not null,
	note text not null default '',
	checked bool not null default 0,
	internal bool not null default 0
);
insert into transfer (id, date, fromId, toId, reference, amount, note, checked, internal) select id, date, fromId, toId, reference, amount, note, checked, internal from tmp_transfer;
drop table tmp_transfer;
