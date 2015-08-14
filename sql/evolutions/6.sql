# --- !Ups

# add start balance for accounts
alter table account add column startBalance integer not null default 0;

# add invert amount option for formats
alter table format add column invertAmount integer not null default 0;

# --- !Downs

# drop column startBalance
alter table account rename to tmp_account;
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
insert into account (id, isOwn, name, owner, iban, bic, accountNumber, bankCode) select id, isOwn, name, owner, iban, bic, accountNumber, bankCode from tmp_account;
drop table tmp_account;

# drop column invertAmount 
alter table format rename to tmp_format;
create table format (
	id integer primary key autoincrement,
	name text default '',
	hashedHeader text not null,
	delimiter text not null default ";",
	textQualifier text not null default "",
	skipFirstLine integer not null default 0,
	dateFormat text not null default "dd.MM.yy",
	columnPositions text not null,
	lineSuffix text not null
);
insert into format (id, name, hashedHeader, delimiter, textQualifier, skipFirstLine, dateFormat, columnPositions, lineSuffix) select id, name, hashedHeader, delimiter, textQualifier, skipFirstLine, dateFormat, columnPositions, lineSuffix from tmp_format;
drop table tmp_format;
