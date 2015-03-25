<?php // Includes
  require_once ("mysql-api.php");
  require_once ("core.php");
?>
<?php // Definitions
  // Tables
  define ("TABLE_CATEGORY", "Category");
  define ("TABLE_DATATYPE", "DataType");
  define ("TABLE_EXCHANGE", "Exchange");
  define ("TABLE_PROGRESS", "InsertionProgress");
  define ("TABLE_STOCK", "Stock");
  define ("TABLE_DATA", "StockData");
  define ("TABLE_DIVIDEND", "StockDividend");
  define ("TABLE_METADATA", "StockMetaData");
  define ("TABLE_FAILURE", "StockFailure");

  // Custom Fields
  define ("FIELD_COUNT", "Count");

  // Primary Keys
  define ("PK_CATEGORY", "CategoryId");
  define ("PK_DATATYPE", "DataTypeId");
  define ("PK_EXCHANGE", "ExchangeId");
  define ("PK_PROGRESS", "InsertionProgressId");
  define ("PK_STOCK", "StockId");
  define ("PK_DATA", "StockDataId");
  define ("PK_DIVIDEND", "StockDividendId");
  define ("PK_METADATA", "StockMetaDataId");
  define ("PK_FAILURE", "StockFailureId");

  //  Fields
  define ("DT_NAME", "DataTypeName");

  define ("P_START_DATE", "StartDate");
  define ("P_END_DATE", "EndDate");
  define ("P_SIZE", "SetSize");
  define ("P_INDEX", "SetIndex");
  define ("P_WHERE", "WhereClause");

  define ("M_FOUNDED", "Founded");
  define ("M_ALIVE", "IsAlive");
  define ("M_DIVIDENDS", "HasDividends");

  define ("DATA_DATE", "StockDate");
  define ("DATA_HIGH", "High");
  define ("DATA_LOW", "Low");
  define ("DATA_CLOSE", "Close");

  define ("STOCK_TICKER", "Ticker");
  define ("STOCK_NAME", "StockName");

  define ("E_EXCHANGE_NAME", "ExchangeName");

  define ("C_CATEGORY_NAME", "CategoryName");

  define ("D_DIVIDEND_DATE", "DividendDate");
  define ("D_DIVIDEND_PRICE", "DividendPrice");

  define ("F_Start", "FailureStartDate");
  define ("F_END", "FailureEndDate");

  // Default Values
  define ("DEFAULT_PAGE_COUNT", 100);
  define ("DEFAULT_PAGE", 0);
?>
<?php // Classes
  abstract class SecurityTable
  {
    protected abstract function GetTableName ();
    protected abstract function GetPrimaryKey ();
    protected function GetContents () { return "*"; }
    protected function GetFrom () { return $this->GetTableName (); }

    // Public Methods
    public function Get ($where = "", $orderBy = "", $count = "", $skip = "")
    {
      $from = $this->GetFrom ();
      $contents = $this->GetContents ();

      return SELECT ($contents, $from, $where, $orderBy, $count, $skip);
    }

    public function GetByPrimary ($key)
    {
      ExpectId ($key);
      $primaryKey = $this->GetPrimaryKey ();
      $tableName = $this->GetTableName ();

      $where = sprintf ("%s.%s = %s", $tableName, $primaryKey, $key);
      return $this->GetSingle ($where);
    }

    public function GetPaged ($page = DEFAULT_PAGE, $count = DEFAULT_PAGE_COUNT, $where = "")
    {
      if (!is_numeric ($page) || $page < 0)
        $page = DEFAULT_PAGE;

      if (!is_numeric ($count) || $count < 1)
        $count = DEFAULT_PAGE_COUNT;

      $skip = $page * $count;
      $orderBy = $this->GetPrimaryKey ();
    
      return $this->Get ($where, $orderBy, $count, $skip);
    }

    public function GetCount ($where = "")
    {
      $from = $this->GetFrom ();
      $contents = sprintf ("COUNT(*) as %s", FIELD_COUNT);
      $response = Single (SELECT ($contents, $from, $where));

      return ValueGetCritical ($response, FIELD_COUNT);
    }

    public function DeleteByPrimary ($key)
    {
      ExpectId ($key);

      $from = $this->GetTableName ();
      $pk = $this->GetPrimaryKey ();
      $where = sprintf ("%s = %d", $pk, $key);

      $this->DeleteBase ($where);
    }

    public function DeleteByForeign ($keyName, $keyValue)
    {
      Expect ($keyName, "Must provide the name of the foreign key to delete by.");
      ExpectId ($keyValue);

      $where = sprintf("%s = %d", $keyName, $keyValue);
      $this->DeleteBase ($where);
    }

    // Protected Methods
    protected function GetSingle ($where = "", $orderBy = "", $count = "", $skip = "")
    {
      return Single ($this->Get ($where, $orderBy, $count, $skip));
    }

    protected function GetAfterDate ($dateName, $dateValue)
    {
      Expect ($dateName, "Must specify the date field to search by.");
      Expect ($dateValue, "Must specify the date to search after.");

      $where = sprintf ("%s > %s", $dateName, DateSqlFormat ($dateValue)); 
      $orderBy = $dateName;

      return $this->Get ($where, $orderBy);
    }

    protected function GetLatestDateBase ($dateName)
    {
      Expect ($dateName, "Must provide a date name.");
      $where = "";
      $orderBy = sprintf ("%s DESC", $dateName);
      $count = 1;

      if (!($latestDividend = $this->GetSingle($where, $orderBy, $count)))
        return false;

      return ValueGetCritical ($latestDividend, $dateName);
    }

    protected function DeleteByDate ($dateName, $dateValue)
    {
      Expect ($dateName, "Must specify the date field to search by.");
      Expect ($dateValue, "Must specify the date to delete after.");

      $where = sprintf ("%s > '%s'", $dateName, $dateValue);
      $this->DeleteBase ($where);
    }

    protected function UpdateBase ($modify, $where = "")
    {
      $from = $this->GetTableName ();
      UPDATE ($from, $modify, $where);
    }

    protected function InsertBase ($columns, $values)
    {
      $from = $this->GetTableName ();
      INSERT ($from, $columns, $values);
    }

    protected function DeleteBase ($where)
    {
      Expect ($where, "Must specify a where clause for deletion.");
      
      $from = $this->GetTableName ();
      DELETE ($from, $where);
    }
  }

  class SecurityCategory extends SecurityTable
  {
    protected function GetTableName () { return TABLE_CATEGORY; }
    protected function GetPrimaryKey () { return PK_CATEGORY; }
  }

  class SecurityDataType extends SecurityTable
  {
    protected function GetTableName () { return TABLE_DATATYPE; }
    protected function GetPrimaryKey () { return PK_DATATYPE; }
  }

  class SecurityExchange extends SecurityTable
  {
    protected function GetTableName () { return TABLE_EXCHANGE; }
    protected function GetPrimaryKey () { return PK_EXCHANGE; }
  }

  class SecurityProgress extends SecurityTable
  {
    protected function GetTableName () { return TABLE_PROGRESS; }
    protected function GetPrimaryKey () { return PK_PROGRESS; }

    protected function GetFrom ()
    {
      return sprintf ("%s JOIN %s ON %s.%s = %s.%s",
        TABLE_PROGRESS,
        TABLE_DATATYPE, 
        TABLE_PROGRESS, PK_DATATYPE, 
        TABLE_DATATYPE, PK_DATATYPE);
    }

    protected function GetContents ()
    {
      return sprintf ("%s,%s,%s,%s,%s,%s,%s,%s.%s", PK_PROGRESS, P_START_DATE, P_END_DATE, 
        P_SIZE, P_INDEX, DT_NAME, P_WHERE, TABLE_PROGRESS, PK_DATATYPE);
    }

    public function GetByDataType ($dataType)
    {
      Expect ($dataType, "Must provide a data type to search for.");
      $where = sprintf ("%s = '%s'", DT_NAME, $dataType);

      return $this->GetSingle ($where);
    }

    public function Insert ($dataTypeId, $setSize, $setIndex, $startDate = NULL, $endDate = NULL, $where = "")
    {
      ExpectId ($dataTypeId);
      ExpectUint ($setSize);
      ExpectUint ($setIndex);

      $columns = sprintf ("(%s,%s,%s,%s,%s,%s)", P_START_DATE, P_END_DATE, P_SIZE, P_INDEX, PK_DATATYPE, P_WHERE);
      $values = sprintf ("(%s,%s,%d,%d,%d,'%s')", DateSqlFormat ($startDate), DateSqlFormat ($endDate), $setSize, $setIndex, $dataTypeId, $where);
      
      $this->InsertBase ($columns, $values);
    }

    public function UpdateIndex ($dataTypeId, $index)
    {
      ExpectUint ($index);
      $modify = sprintf ("%s = %d", P_INDEX, $index);
      $where = sprintf ("%s = %d", PK_DATATYPE, $dataTypeId);

      $this->UpdateBase ($modify, $where);
    }

    public function DeleteByDataTypeId ($dataTypeId)
    {
      ExpectId ($dataTypeId, "Must provide a data type id to delete by.");
      $where = sprintf ("%s = %d", PK_DATATYPE, $dataTypeId);
      $this->DeleteBase ($where);
    }
  }

  class SecurityStock extends SecurityTable
  {
    protected function GetTableName () { return TABLE_STOCK; }
    protected function GetPrimaryKey () { return PK_STOCK; }

    protected function GetFrom () 
    { 
      return sprintf ("%s JOIN %s ON %s.%s = %s.%s JOIN %s ON %s.%s = %s.%s LEFT JOIN %s on %s.%s = %s.%s",
        TABLE_STOCK, 
        TABLE_EXCHANGE, TABLE_STOCK, PK_EXCHANGE, TABLE_EXCHANGE, PK_EXCHANGE,
        TABLE_CATEGORY, TABLE_STOCK, PK_CATEGORY, TABLE_CATEGORY, PK_CATEGORY,
        TABLE_METADATA, TABLE_STOCK, PK_STOCK, TABLE_METADATA, PK_STOCK);
    }

    protected function GetContents ()
    {
      return sprintf ("%s.%s,%s,%s,%s,%s,%s,%s,%s", TABLE_STOCK, PK_STOCK, STOCK_TICKER, STOCK_NAME, 
        E_EXCHANGE_NAME, C_CATEGORY_NAME, M_FOUNDED, M_ALIVE, M_DIVIDENDS);
    }

    public function GetNewerThan ($founded)
    {
      return $this->GetAfterDate (M_FOUNDED, $founded);
    }
  }

  class SecurityData extends SecurityTable
  {
    protected function GetTableName () { return TABLE_DATA; }
    protected function GetPrimaryKey () { return PK_DATA; }

    public function GetNewerThan ($date)
    {
      return $this->GetAfterDate (DATA_DATE, $date);
    }

    public function GetLatestDate ()
    {
      return $this->GetLatestDateBase (DATA_DATE);
    }

    public function DeleteAfter ($date)
    {
      return $this->DeleteByDate (DATA_DATE, $date);
    }

    public function Insert ($stockId, $date, $high, $low, $close)
    {
      ExpectId ($stockId);
      ExpectDate ($date);
      ExpectNumeric ($high);
      ExpectNumeric ($low);
      ExpectNumeric ($close);

      $columns = sprintf ("(%s,%s,%s,%s,%s)", PK_STOCK, DATA_DATE, DATA_HIGH, DATA_LOW, DATA_CLOSE);
      $values = sprintf ("(%d,%s,%f,%f,%f)", $stockId, DateSqlFormat ($date), $high, $low, $close);

      return $this->InsertBase ($columns, $values);
    }
  }

  class SecurityDividend extends SecurityTable
  {
    protected function GetTableName () { return TABLE_DIVIDEND; }
    protected function GetPrimaryKey () { return PK_DIVIDEND; }

    public function GetNewerThan ($date)
    {
      return $this->GetAfterDate (D_DIVIDEND_DATE, $date);
    }

    public function GetLatestDate ()
    {
      return $this->GetLatestDateBase (D_DIVIDEND_DATE);
    }

    public function DeleteAfter ($date)
    {
      return $this->DeleteByDate (D_DIVIDEND_DATE, $date);
    }

    public function Insert ($stockId, $date, $dividend)
    {
      ExpectId ($stockId);
      ExpectDate ($date);
      ExpectNumeric ($dividend);

      $columns = sprintf ("(%s,%s,%s)", PK_STOCK, D_DIVIDEND_DATE, D_DIVIDEND_PRICE);
      $values = sprintf ("(%d,'%s',%f)", $stockId, $date, $dividend);

      $this->InsertBase ($columns, $values);
    }
  }

  class SecurityMeta extends SecurityTable
  {
    protected function GetTableName () { return TABLE_METADATA; }
    protected function GetPrimaryKey () { return PK_METADATA; }

    public function Insert ($stockId, $founded, $isAlive, $hasDividends)
    {
      ExpectId ($stockId);     
      ExpectDate ($founded);
      ExpectBool ($isAlive);
      ExpectBool ($hasDividends);

      $columns = sprintf ("(%s,%s,%s,%s)", PK_STOCK, M_FOUNDED, M_ALIVE, M_DIVIDENDS);
      $values = sprintf ("(%d,%s,%s,%s)", $stockId, DateSqlFormat ($founded), BoolFormat ($isAlive), BoolFormat ($hasDividends));

      $this->InsertBase ($columns, $values);
    }
  }

  class SecurityFailure extends SecurityTable
  {
    protected function GetTableName () { return TABLE_FAILURE; }
    protected function GetPrimaryKey () { return PK_FAILURE; }

    public function GetByStockId ($stockId)
    {
      ExpectId ($stockId);
    
      $where = sprintf ("%s = %d", PK_STOCK, $stockId);
      
      return $this->Get ($where);
    }

    public function Delete ($stockId, $dataTypeId, $startDate, $endDate)
    {
      ExpectId ($stockId);
      ExpectId ($dataTypeId);

      $where = sprintf ("%s = %d AND %s = %d AND %s = %s AND %s = %s",
        PK_STOCK, $stockId,
        PK_DATATYPE, $dataTypeId,
        F_START, DateSqlFormat ($startDate),
        F_End, DateSqlFormat ($endDate));

      $this->DeleteBase ($where);
    }

    public function Insert ($stockId, $dataTypeId, $startDate, $endDate)
    {
      ExpectId ($stockId);
      ExpectId ($dataTypeId);

      $columns = sprintf ("(%s,%s,%s,%s)", PK_STOCK, PK_DATATYPE, F_START, F_END);
      $values = sprintf ("(%d,%d,%s,%s)", $stockId, $dataTypeId, DateSqlFormat ($startDate), DateSqlFormat ($endDate));

      $this->InsertBase ($columns, $values);
    }
  }

  class SecurityDAL
  {
    public $Category;
    public $DataType;
    public $Exchange;
    public $Progress;
    public $Stock;
    public $Data;
    public $Dividend;
    public $Meta;
    public $Failure;

    function __construct()
    {
      $this->Category = new SecurityCategory ();
      $this->DataType = new SecurityDataType ();
      $this->Exchange = new SecurityExchange ();
      $this->Progress = new SecurityProgress ();
      $this->Stock = new SecurityStock ();
      $this->Data = new SecurityData ();
      $this->Dividend = new SecurityDividend ();
      $this->Meta = new SecurityMeta ();
      $this->Failure = new SecurityFailure ();
    }
  }
?>
