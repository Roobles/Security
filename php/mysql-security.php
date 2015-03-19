<?php // Includes
  require_once ("mysql-api.php");
  require_once ("core.php");
?>
<?php // Definitions
  define ("TABLE_CATEGORY", "Category");
  define ("TABLE_DATATYPE", "DataType");
  define ("TABLE_EXCHANGE", "Exchange");
  define ("TABLE_PROGRESS", "InsertionProgress");
  define ("TABLE_STOCK", "Stock");
  define ("TABLE_DATA", "StockData");
  define ("TABLE_DIVIDEND", "StockDividend");
  define ("TABLE_META", "StockMetaData");
?>
<?php
  class DbSecurity
  {
    // Private Members
    private $Tables = array 
    (
      TABLE_CATEGORY => "Category",
      TABLE_DATATYPE => "DataType",
      TABLE_EXCHANGE => "Exchange",
      TABLE_PROGRESS => "InsertionProgress",
      TABLE_STOCK => "Stock",
      TABLE_DATA => "StockData",
      TABLE_DIVIDEND => "StockDividend",
      TABLE_META => "StockMetaData"
    );

    // Public Functions
  }
?>
