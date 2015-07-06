
# --- !Ups

create table tag (
	id integer primary key autoincrement,
	name varchar(64)
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

# --- !Downs

drop table tag;
drop table accountTag;
drop table transferTag;