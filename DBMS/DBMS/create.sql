CREATE TABLE publisher (
  id int(10) NOT NULL,
  name varchar(100) NOT NULL,
  nation varchar(3),
  PRIMARY KEY  (id)
);

INSERT INTO publisher VALUES 
 (100008,'Oxbow Books Limited','PRC'),
 (100010,'Aardwolf Publishing','PRC'),
 (100012,'Miguel Angel Porrua','PRC');