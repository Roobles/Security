CREATE DATABASE IF NOT EXISTS security;
USE security;

DROP TABLE IF EXISTS Stock;
DROP TABLE IF EXISTS Category;
DROP TABLE IF EXISTS Exchange;

CREATE TABLE Exchange (
  ExchangeId SMALLINT NOT NULL AUTO_INCREMENT,
  ExchangeName CHAR(3) NOT NULL,
  PRIMARY KEY (ExchangeId),
  UNIQUE (ExchangeName));

CREATE TABLE Category (
  CategoryId SMALLINT NOT NULL AUTO_INCREMENT,
  CategoryName VARCHAR(50),
  PRIMARY KEY (CategoryId),
  UNIQUE (CategoryName));

CREATE TABLE Stock (
 StockId INT NOT NULL AUTO_INCREMENT,
 Ticker VARCHAR(10) NOT NULL,
 StockName VARCHAR(50) NOT NULL,
  ExchangeId SMALLINT NOT NULL,
  CategoryId SMALLINT NOT NULL,
  PRIMARY KEY (StockId),
  FOREIGN KEY (ExchangeId) REFERENCES Exchange(ExchangeId),
  FOREIGN KEY (CategoryId) REFERENCES Category(CategoryId),
  CONSTRAINT uc_StockContents UNIQUE (Ticker,StockName));

source ~/git/security/sql/security_data.sql
