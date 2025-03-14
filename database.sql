CREATE DATABASE supermarket;

USE supermarket;

CREATE TABLE products (
    product_id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255),
    price DOUBLE,
    stock INT
);

SHOW TABLES;
