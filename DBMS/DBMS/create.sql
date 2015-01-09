CREATE DATABASE orderDB;
CREATE DATABASE orderDB1;
DROP DATABASE orderDB1;
USE orderDB;

CREATE TABLE publisher (
  id int(10) NOT NULL,
  name varchar(100) NOT NULL,
  nation varchar(3),
  PRIMARY KEY  (id)
);

DESC publisher;
CREATE TABLE book (
  id int(10) NOT NULL,
  title varchar(100) NOT NULL,
  authors varchar(200),
  publisher_id int(10) NOT NULL,
  copies int(10),
  PRIMARY KEY  (id)
);
DROP TABLE book;
CREATE TABLE customer (
  id int(10) NOT NULL,
  name varchar(25) NOT NULL,
  rank int(10) NOT NULL,
  PRIMARY KEY  (id)
);

CREATE TABLE orders (
  customer_id int(10) NOT NULL,
  book_id int(10) NOT NULL,
  quantity int(10) NOT NULL
);
DESC orders;
SHOW TABLES;