
create table evolution (
	id integer primary key,
	upTs integer not null default 0,
	downTs integer not null default 0,
	ups text not null default '',
	downs text not null default ''
);

create table account (
	id integer primary key autoincrement,
	isOwn bool not null default 0,
	balance int not null default 0,
	name text not null default '',
	owner text not null,
	iban text not null default '',
	bic text not null default '',
	accountNumber text not null default '',
	bankCode text not null default '',
	initialBalance integer not null default 0,
	currency text not null default 'EUR'
);

create table transfer (
	id integer primary key autoincrement,
	date integer not null default 0,
	ymd date not null default '1970-01-01',
	fromId integer not null,
	toId integer not null,	
	reference text not null,
	amount integer not null,
	currency text not null default 'EUR'
	note text not null default '',
	note text not null default '',
	checked bool not null default 0,
	internal bool not null default 0
);

create table tag (
	id integer primary key autoincrement,
	name text
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

create table format (
	id integer primary key autoincrement,
	name text default '',
	hashedHeader text not null,
	delimiter text not null default ";",
	textQualifier text not null default "",
	skipFirstLine bool not null default 0,
	dateFormat text not null default "dd.MM.yy",
	columnPositions text not null,
	lineSuffix text not null,
	invertAmount bool not null default 0
);

create table file (
	id integer primary key autoincrement,
	path text not null,
	formatId integer not null,
	watch bool not null default 1,
	lastImport integer not null default 0,
	lastImportHash text not null default ''
);