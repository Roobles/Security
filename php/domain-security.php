<?php // Includes
  require_once ("mysql-security.php");
  require_once ("yahoo-finance.php");
  require_once ("mock.php");
?>
<?php // Constants
  // TODO: Consider making these parameters to the constructor.
  define ("YEARS_BACK", 5);
  define ("UPDATE_BLOCK_SIZE", 5);

  // Data Access Layers
  define ("SECURITY_DAL", "SecurityDAL");
  define ("YAHOO_DAL", "YahooFinance");
  define ("YAHOO_DAL_MOCK", "MockYahooFinance");
?>
<?php // Class
  class Security 
  {
    // Private Members
    private $SecurityDAL;
    private $YahooFinance;
    private $ValidDataTypes;

    private $YearsBack = YEARS_BACK;

    // Constructor
    function __construct ($securityDal = SECURITY_DAL, $yahooDal = YAHOO_DAL)
    {
      $this->SecurityDAL = new $securityDal ();
      $this->YahooFinance = new $yahooDal ();
      $this->SetValidDataTypes ();
    }

    // Public Methods
    public function Test()
    {
      $security = $this->SecurityDAL;
      $yahoo = $this->YahooFinance;

      $this->RepairData ();
    }

    public function UpdateDividends ()
    {
      $dataType = TABLE_DIVIDEND;
      $updateSource = $this->SecurityDAL->Stock;
      $updateFunction = "UpdateDividendSingle";
      $where = sprintf ("%s = 1", M_DIVIDENDS);

      $this->UpdateTable ($dataType, true, $updateSource, $updateFunction, $where);
    }

    public function UpdateStocks ()
    {
      $dataType = TABLE_DATA;
      $updateSource = $this->SecurityDAL->Stock;
      $updateFunction = "UpdateStockDataSingle";
      $where = "Stock.StockId IS NOT NULL";

      $this->UpdateTable ($dataType, true, $updateSource, $updateFunction, $where);
    }

    public function RepairData ()
    {
      $dataType = TABLE_FAILURE;
      $updateSource = $this->SecurityDAL->Failure;
      $updateFunction = "RepairDataSingle";
      $repairRetries = 50;
      $originalRetries = GetYahooMaxRetries ();

      // TODO: After this round, make things more robust.
      ExpectId ($repairTableId = $this->IsValidDataType (TABLE_FAILURE));
      $where = sprintf ("%s <> %d", PK_DATATYPE, $repairTableId);


      SetYahooMaxRetries ($repairRetries);
      $this->UpdateTable ($dataType, false, $updateSource, $updateFunction, $where);
      SetYahooMaxRetries ($originalRetries);
    }

    public function RepairMetaData ()
    {
      $where = "Founded IS NULL";
      $repairRetries = 8;
      $originalRetries = GetYahooMaxRetries ();

      SetYahooMaxRetries ($repairRetries);
      $this->UpdateMetaDataBase ($where);
      SetYahooMaxRetries ($originalRetries);
    }

    public function UpdateMetaData ()
    {
      $this->UpdateMetaDataBase ("");
    }

    // Private Methods
    private function UpdateMetaDataBase ($where = "")
    {
      $dataType = TABLE_METADATA;
      $updateSource = $this->SecurityDAL->Stock;
      $updateFunction = "UpdateMetaDataSingle";

      $this->UpdateTable ($dataType, false, $updateSource, $updateFunction, $where);
    }

    // Common Private Methods
    private function UpdateTable ($dataType, $hasDates, $updateSourceObj, $updateFunction, $where = "")
    {
      $page; $count; $startDate; $endDate; $dataTypeId; $sourceCount;
      $progress = $this->SecurityDAL->Progress;

      Message (sprintf ("Updating table '%s'.", $dataType));
      $this->RecoverUpdateParameters ($dataType, $hasDates, $page, $count, $startDate, $endDate, $dataTypeId, $where);
      $sourceCount = $updateSourceObj->GetCount ($where);

      // Main Loop.
      while ((($updateSources = $updateSourceObj->GetPaged ($page, $count, $where)) && count ($updateSources) > 0))
      {
        $this->UpdateTableRange ($updateSources, $updateFunction, $dataType, $dataTypeId, $page, $count, $sourceCount, $startDate, $endDate);
        $progress->UpdateIndex ($dataTypeId, ++$page);
      }

      Message (sprintf ("Finished updating table '%s'.", $dataType));
      $progress->DeleteByDataTypeId ($dataTypeId);
    }

    private function UpdateTableRange ($updateSources, $updateFunction, $dataType, $dataTypeId, $page, $count, $sourceCount, $startDate, $endDate)
    {
      $bottomRange = $page * $count;
      $topRange = $bottomRange + $count;
      $totalItterations = $sourceCount / $count + (($sourceCount % $count) > 0 ? 1 : 0);
      $currentItteration = $page + 1;

      $message = sprintf ("Updating '%s' range (%d - %d) itteration (%d of %d).", $dataType, $bottomRange, $topRange, $currentItteration, $totalItterations);
      Message ($message);

      foreach ($updateSources as $updateSource)
        $this->AttemptMethod ($updateFunction, $updateSource, $dataTypeId, $startDate, $endDate);
    }

    private function AttemptMethod ($methodName, $methodParam, $dataTypeId, $startDate, $endDate)
    {
      try
      {
        if (!$this->$methodName($methodParam, $startDate, $endDate))
          $this->RecordFailure ($methodParam, $dataTypeId, $startDate, $endDate);
      }
      catch (Exception $e)
      { 
        $errMessage = sprintf ("Call to '%s' failed, with error message: %s", $methodName, $e->getMessage ());
        Error ($errMessage);
      }
    }

    // Reads in old values, if existing, and creates 
    private function RecoverUpdateParameters ($dataType, $hasDates, &$page, &$count, &$startDate, &$endDate, &$dataTypeId, &$where)
    {
      $page = 0;
      $count = UPDATE_BLOCK_SIZE;
      $startDate = $hasDates ? $this->GetStartDateForDataType ($dataType): NULL;
      $endDate = $hasDates ? $this->GetEndDateForDataType ($dataType, $startDate) : NULL;
      $progress = $this->SecurityDAL->Progress;

      // Verify that provided data type is valid.
      if (!$dataTypeId = $this->IsValidDataType ($dataType))
        Error (sprintf ("Invalid Data Type ('%s') provided.", $dataType));

      // Check if an update was in progress that became interrupted.
      if ($existingProgress = $progress->GetByDataType ($dataType))
      {
        $page = ValueGetCritical ($existingProgress, P_INDEX);
        $count = ValueGetCritical ($existingProgress, P_SIZE);
        $where = ValueGet ($existingProgress, P_WHERE);

        if ($hasDates)
        {
          $startDate = ValueGetCritical ($existingProgress, P_START_DATE);
          $endDate= ValueGetCritical ($existingProgress, P_END_DATE);
         }

        Message (sprintf ("Recovering prior update, starting at range %d.", $page * $count));
        $progress->DeleteByDataTypeId ($dataTypeId);
      }

      $progress->Insert ($dataTypeId, $count, $page, $startDate, $endDate, $where);
    }

    private function GetStartDateForDataType ($dataType)
    {

      $defaultStart = ($dataType == TABLE_DIVIDEND)
        ? DateFormat ("01/01/1960")
        : DateFormat ("01/01/2010");

      $targetObject = ($dataType == TABLE_DIVIDEND)
        ? $this->SecurityDAL->Dividend
        : $this->SecurityDAL->Data;

      if (!($startDate = $targetObject->GetLatestDate ()))
        $startDate = $defaultStart;

      return $startDate;
    }

    private function GetEndDateForDataType ($dataType, $startDate)
    {
      if ($dataType == TABLE_DIVIDEND)
        return DateFormat (DateGetToday ());

      // Assumed that data type is now StockHistory
      $timeSpanInYears = 1;
      $endDate = DateAddYears ($startDate, $timeSpanInYears);
      $today = DateGetToday ();

      return (DateCompare ($endDate, $today) > 0)
        ? $today
        : $endDate;
    }

    private function SetValidDataTypes ()
    {
      $this->ValidDataTypes = array ();
      $dbTypes = $this->SecurityDAL->DataType->Get ();
      foreach ($dbTypes as $dbType)
        array_push ($this->ValidDataTypes, array (PK_DATATYPE => ValueGetCritical ($dbType, PK_DATATYPE), DT_NAME => ValueGetCritical ($dbType, DT_NAME)));
    }

    private function IsValidDataType ($dataType)
    {
      foreach ($this->ValidDataTypes as $validType)
        if (ValueGetCritical ($validType, DT_NAME) == $dataType)
          return ValueGetCritical ($validType, PK_DATATYPE);

      return false;
    }

    private function GetValidDataType ($dataTypeId)
    {
      foreach ($this->ValidDataTypes as $validType)
        if (ValueGetCritical ($validType, PK_DATATYPE) == $dataTypeId)
          return ValueGetCritical ($validType, DT_NAME);

      return false;
    }

    private function RecordFailure ($paramObj, $dataTypeId, $startDate, $endDate)
    {
      $stockId = ValueGetCritical ($paramObj, PK_STOCK);  
      
      $message = sprintf ("Recording failure for Stock Id '%d'.", $stockId);
      Message ($message);
      
      $this->SecurityDAL->Failure->Insert ($stockId, $dataTypeId, $startDate, $endDate);
    }

    
    // Domain Specific Methods
    private function UpdateDividendSingle ($stockObj, $startDate, $endDate)
    {
      $stockId = ValueGetCritical ($stockObj, PK_STOCK);
      $symbol = ValueGetCritical ($stockObj, STOCK_TICKER);
      $divid = $this->SecurityDAL->Dividend;

      if (!($results = $this->YahooFinance->GetDividendHistory ($symbol, $startDate, $endDate)))
      {
        Warn (sprintf ("Could not retrieve dividend information for stock %s.", $symbol));
        return false;
      }

      foreach ($results as $result)
      {
        $dividend = ValueGetCritical ($result, YD_DIVIDENDS);
        $date = ValueGetCritical ($result, YH_DATE);

        $divid->Insert ($stockId, $date, $dividend);
      }
      
      return true;
    }

    private function UpdateStockDataSingle ($stockObj, $startDate, $endDate)
    {
      $stockId = ValueGetCritical ($stockObj, PK_STOCK);
      $symbol = ValueGetCritical ($stockObj, STOCK_TICKER);
      $founded = ValueGetCritical ($stockObj, M_FOUNDED);
      $isAlive = ValueGetCritical ($stockObj, M_ALIVE);

      $shouldHaveData = DateCompare ($endDate, $founded) > 0;
      if (!$isAlive || !$shouldHaveData)
        return true;

      $data = $this->SecurityDAL->Data;
      if (!($results = $this->YahooFinance->GetStockHistory ($symbol, $startDate, $endDate)))
      {
        Warn (sprintf ("Could not retrieve history information for stock %s.", $symbol));
        return false;
      }

      foreach ($results as $result)
      {
        $date = ValueGetCritical ($result, YH_DATE);
        $high = ValueGetCritical ($result, YH_HIGH);
        $low = ValueGetCritical ($result, YH_LOW);
        $close = ValueGetCritical ($result, YH_CLOSE);

        $data->Insert ($stockId, $date, $high, $low, $close);
      } 

      return true;
    }

    private function UpdateMetaDataSingle ($stockObj, $startDate, $endDate)
    {
      $stockId = ValueGetCritical ($stockObj, PK_STOCK);
      $symbol = ValueGetCritical ($stockObj, STOCK_TICKER);

      // Delete old existing data.
      $this->SecurityDAL->Meta->DeleteByForeign (PK_STOCK, $stockId);
  
      // Retrieve yahoo object.  Or fail out.
      if (!($yahooMeta = $this->YahooFinance->GetMetaData ($symbol)))
      {
        Warn (sprintf ("Could not retrieve meta data for stock '%s'", $symbol));
        return false;
      }

      // Parse retrieved meta data object.
      $startDate = DateFormat (ValueGetCritical ($yahooMeta, YM_START, false));
      $endDate = DateFormat (ValueGetCritical ($yahooMeta, YM_END, false));
      $isAlive = $this->IsAlive ($endDate);
      $hasDividends = $this->HasDividends ($symbol, $startDate);

      $this->SecurityDAL->Meta->Insert ($stockId, $startDate, $isAlive, $hasDividends);
      return true;
    }

    private function RepairDataSingle ($failureObj, &$startDate, &$endDate)
    {
      $stockId = ValueGetCritical ($failureObj, PK_STOCK);
      $dataTypeId = ValueGetCritical ($failureObj, PK_DATATYPE);
      $startDate = ValueGetCritical ($failureObj, F_START);
      $endDate = ValueGetCritical ($failureObj, F_END);

      $dataType = $this->GetValidDataType ($dataTypeId);
      $stockObj = $this->SecurityDAL->Stock->GetByPrimary ($stockId);
      $updateMethod = NULL;

      switch ($dataType)
      {
        case TABLE_DATA:
          $updateMethod = "UpdateStockDataSingle";
          break;
      }

      if ($updateMethod == NULL)
        Error ("Invalid data type was stored in stock failure");

      return $this->$updateMethod ($stockObj, $startDate, $endDate);
    }

    private function IsAlive ($endDate)
    {
      $today = DateGetToday ();
      return (DateCompare ($endDate, $today) >= 0);
    }

    private function HasDividends ($symbol, $startDate)
    {
      $dividends = $this->YahooFinance->GetDividendHistory ($symbol, $startDate);
      return (is_array ($dividends) && count ($dividends) > 0);
    }
  }
?>
