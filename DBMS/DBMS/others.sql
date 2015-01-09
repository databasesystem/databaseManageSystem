CREATE DATABASE orderDB;
DROP DATABASE orderDB;
DROP TABLE customer;
USE orderDB;
SHOW TABLES;
DESC customer;
CREATE TABLE customer (
id int(10) NOT NULL,
name varchar(25) NOT NULL,
rank int(10) NOT NULL,
PRIMARY KEY (id)
);

