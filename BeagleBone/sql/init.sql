create table moisture( id INTEGER, time INTEGER, value INTEGER, PRIMARY KEY( id, time ) );
create table humidity( id INTEGER, time INTEGER, value INTEGER, PRIMARY KEY( id, time ) );
create table temperature( id INTEGER, time INTEGER, value INTEGER, PRIMARY KEY( id, time ) );
create table pid(id INTEGER, time INTEGER, derivativeState REAL, integratorState REAL, pumpDuration INTEGER, PRIMARY KEY(id , time));
create table devices(id INTEGER, status TEXT, PRIMARY KEY(id));
