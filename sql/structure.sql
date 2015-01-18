
create table evolution (
	id integer primary key,
	upTs integer not null default 0,
	downTs integer not null default 0,
	ups text not null,
	downs text not null
);

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

create table transfer (
	id integer primary key autoincrement,
	date integer not null,
	fromId integer not null,
	toId integer not null,
	reference text not null,
	amount integer not null,
	note text not null default '',
	internal bool not null default 0,
	checked bool not null default 0
);

create table tag (
	id integer primary key autoincrement,
	name varchar(32)
);

create table accountTag (
	tagId integer not null,
	accountId integer not null,
	primary key (tagId, accountId)
);

create table transferTag (
	tagId integer not null,
	transferId integer not null,
	primary key (tagId, transferId)
);

-- create table budget (
-- 	id integer primary key autoincrement,
-- 	name varchar(64) not null,
-- 	amount integer not null,
-- 	keepSavings bool not null default 1
-- );
