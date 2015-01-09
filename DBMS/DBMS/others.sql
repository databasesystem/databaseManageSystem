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
INSERT INTO orders VALUES
 (304403,209743,2);
DELETE FROM publisher WHERE nation=’USA’;

UPDATE book SET title=’Nine Times Nine’ WHERE authors=’Anthony Boucher’;
SELECT title FROM book WHERE authors is null;
SELECT book.title,orders.quantity FROM book,orders WHERE book.id=orders.book_id AND orders.quantity>8;

