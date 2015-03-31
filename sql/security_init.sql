CREATE DATABASE IF NOT EXISTS security;
USE security;

-- DROP TABLE IF EXISTS StockFailure;

-- DROP TABLE IF EXISTS StockData;
-- DROP TABLE IF EXISTS StockMetaData;
-- DROP TABLE IF EXISTS StockDividend;

-- DROP TABLE IF EXISTS Stock;
-- DROP TABLE IF EXISTS Category;
-- DROP TABLE IF EXISTS Exchange;

DROP TABLE IF EXISTS InsertionProgress;
DROP TABLE IF EXISTS DataType;

CREATE TABLE IF NOT EXISTS Exchange (
  ExchangeId SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
  ExchangeName CHAR(3) NOT NULL,
  PRIMARY KEY (ExchangeId),
  UNIQUE (ExchangeName));

CREATE TABLE IF NOT EXISTS Category (
  CategoryId SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
  CategoryName VARCHAR(50),
  PRIMARY KEY (CategoryId),
  UNIQUE (CategoryName));

CREATE TABLE IF NOT EXISTS Stock (
  StockId SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
  Ticker VARCHAR(10) NOT NULL,
  StockName VARCHAR(50) NOT NULL,
  ExchangeId SMALLINT UNSIGNED NOT NULL,
  CategoryId SMALLINT UNSIGNED NOT NULL,
  PRIMARY KEY (StockId),
  FOREIGN KEY (ExchangeId) REFERENCES Exchange(ExchangeId),
  FOREIGN KEY (CategoryId) REFERENCES Category(CategoryId),
  CONSTRAINT uc_StockContents UNIQUE (Ticker,StockName));

CREATE TABLE IF NOT EXISTS StockMetaData (
  StockMetaDataId SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
  Founded DATE NOT NULL,
  IsAlive BOOLEAN NOT NULL,
  HasDividends BOOLEAN NOT NULL,
  StockId SMALLINT UNSIGNED NOT NULL,
  PRIMARY KEY (StockMetaDataId),
  FOREIGN KEY (StockId) REFERENCES Stock(StockId),
  UNIQUE (StockId));

-- History Data Tables

CREATE TABLE IF NOT EXISTS StockData (
  StockDataId INT UNSIGNED NOT NULL AUTO_INCREMENT,
  StockDate DATE NOT NULL,
  High DECIMAL(8,2) NOT NULL,
  Low DECIMAL(8,2) NOT NULL,
  Close DECIMAL(8,2) NOT NULL,
  StockId SMALLINT UNSIGNED NOT NULL,
  PRIMARY KEY (StockDataId),
  FOREIGN KEY (StockId) REFERENCES Stock(StockId),
  CONSTRAINT uc_DailyStockEntry UNIQUE (StockId,StockDate));

CREATE TABLE IF NOT EXISTS StockDividend (
  StockDividendId INT UNSIGNED NOT NULL AUTO_INCREMENT,
  DividendDate DATE NOT NULL,
  DividendPrice DECIMAL (9,6) NOT NULL,
  StockId SMALLINT UNSIGNED NOT NULL,
  PRIMARY KEY (StockDividendId),
  FOREIGN KEY (StockId) REFERENCES Stock(StockId),
  CONSTRAINT uc_DailyDividend UNIQUE (StockId,DividendDate));

-- Insertion 

CREATE TABLE IF NOT EXISTS DataType (
  DataTypeId SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
  DataTypeName VARCHAR(30) NOT NULL,
  PRIMARY KEY (DataTypeId),
  UNIQUE (DataTypeName));

CREATE TABLE IF NOT EXISTS InsertionProgress (
  InsertionProgressId SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
  StartDate DATE,
  EndDate DATE,
  SetSize SMALLINT UNSIGNED NOT NULL,
  SetIndex SMALLINT UNSIGNED NOT NULL,
  DataTypeId SMALLINT UNSIGNED NOT NULL,
  WhereClause VARCHAR(300) NOT NULL,
  PRIMARY KEY (InsertionProgressId),
  FOREIGN KEY (DataTypeId) REFERENCES DataType(DataTypeId),
  UNIQUE (DataTypeId));

-- Error Tracking

CREATE TABLE IF NOT EXISTS StockFailure (
  StockFailureId INT UNSIGNED NOT NULL AUTO_INCREMENT,
  FailureStartDate DATE,
  FailureEndDate DATE,
  StockId SMALLINT UNSIGNED NOT NULL,
  DataTypeId SMALLINT UNSIGNED NOT NULL,
  PRIMARY KEY (StockFailureId),
  FOREIGN KEY (StockId) REFERENCES Stock(StockId),
  FOREIGN KEY (DataTypeId) REFERENCES DataType(DataTypeId),
  CONSTRAINT uc_StockFailureEntry UNIQUE (StockId,DataTypeId,FailureStartDate,FailureEndDate));

-- Initial Data
INSERT INTO DataType (DataTypeName) VALUES ('StockData'),('StockDividend'),('StockMetaData'),('StockFailure');

-- source ~/git/security/sql/security_data.sql
