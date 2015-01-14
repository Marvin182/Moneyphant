
# --- !Ups

create table tag (
	accountId integer not null default -1,
	transferId integer not null default -1,
	name varchar(32),
	primary key (accountId, transferId, name)
);

# --- !Downs

drop table tag;