<?php // Includes
  require_once ("mysql-security.php");
  require_once ("yahoo-finance.php");
  require_once ("mock.php");
?>
<?php // Constants
  // TODO: Consider making these parameters to the constructor.
  define ("YEARS_BACK", 5);
  define ("UPDATE_BLOCK_SIZE", 10);

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
      //$stock = $this->SecurityDAL->Stock->GetByPrimary (210);
      //$this->UpdateMetaDataSingle ($stock);

      $dataType = "StockMetaData";
      $updateSource = $this->SecurityDAL->Stock;
      $updateFunction = "UpdateMetaDataSingle";
      $this->UpdateTable ($dataType, false, $updateSource, $updateFunction);
    }

    private function UpdateTable ($dataType, $hasDates, $updateSourceObj, $updateFunction)
    {
      $page; $count; $startDate; $endDate; $dataTypeId;
      $progress = $this->SecurityDAL->Progress;

      Message (sprintf ("Updating table '%s'.", $dataType));
      $this->RecoverUpdateParameters ($dataType, $hasDates, $page, $count, $startDate, $endDate, $dataTypeId);

      // Main Loop.
      while ((($updateSources = $updateSourceObj->GetPaged ($page, $count)) && count ($updateSources) > 0))
      {
        $this->UpdateTableRange ($updateSources, $updateFunction, $dataType, $page, $count);
        $progress->UpdateIndex ($dataTypeId, ++$page);
      }
    }

    private function UpdateTableRange ($updateSources, $updateFunction, $dataType, $page, $count)
    {

      $attempts = 30;
      $bottomRange = $page * $count;
      $topRange = $bottomRange + $count;

      $message = sprintf ("Updating '%s' range (%d - %d)", $dataType, $bottomRange, $topRange);
      Message ($message);

      $priorSource = NULL;
      foreach ($updateSources as $updateSource)
      {
        $result = $this->AttemptMethod ($updateFunction, $updateSource, $attempts);
        if (!$result && ($priorSource == NULL || !$this->AttemptMethod ($updateFunction, $priorSource, $attempts)))
          Error (sprintf ("Critical failure updating '%s'.  Try again at another time.", $dataType));

        $priorSource = $updateSource;
      }
    }

    private function AttemptMethod ($methodName, $methodParam, $maxAttempts = 20, $sleep = 1)
    {
      $result = NULL;
      for ($attempts=0; $attempts<$maxAttempts; $attempts++)
      {
        try
        {
          sleep ($sleep);
          $this->$methodName($methodParam);
          return true;
        }
        catch (Exception $e)
        { 
          $errMessage = sprintf ("Call to '%s' failed, with error message: %s\nRetrying (Attempt %d of %d).", $methodName, $e->getMessage (), $attempts, $maxAttempts);
          Warn ($errMessage);
        }
      }
      return false;
    }

    // Reads in old values, if existing, and creates 
    private function RecoverUpdateParameters ($dataType, $hasDates, &$page, &$count, &$startDate, &$endDate, &$dataTypeId)
    {
      $page = 0;
      $count = UPDATE_BLOCK_SIZE;
      $startDate = $hasDates ? GetStartDateForDataType ($dataType): NULL;
      $endDate = $hasDates ? DateFormat (DateGetToday ()) : NULL;
      $progress = $this->SecurityDAL->Progress;

      // Verify that provided data type is valid.
      if (!$dataTypeId = $this->IsValidDataType ($dataType))
        Error (sprintf ("Invalid Data Type ('%s') provided.", $dataType));

      // Check if an update was in progress that became interrupted.
      if ($existingProgress = $progress->GetByDataType ($dataType))
      {
        $page = ValueGetCritical ($existingProgress, P_INDEX);
        $count = ValueGetCritical ($existingProgress, P_SIZE);

        if ($hasDates)
        {
          $startDate = ValueGetCritical ($existingProgress, P_START_DATE);
          $endDate= ValueGetCritical ($existingProgress, P_END_DATE);
        }

        Message (sprintf ("Recovering prior update, starting at range %d.", $page * $count));
        $progress->DeleteByDataTypeId ($dataTypeId);
      }

      $progress->Insert ($dataTypeId, $count, $page, $startDate, $endDate);
    }

    private function GetStartDateForDataType ($dataType)
    {
      // TODO: Implement this.
      return DateFormat (DateGetToday ());
    }

    // Private Methods
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
    
    private function UpdateMetaDataSingle ($stockObj)
    {
      $stockId = ValueGetCritical ($stockObj, PK_STOCK);
      $symbol = ValueGetCritical ($stockObj, "Ticker");

      // Delete old existing data.
      $this->SecurityDAL->Meta->DeleteByForeign (PK_STOCK, $stockId);
  
      // Retrieve yahoo object.
      $yahooMeta = $this->YahooFinance->GetMetaData ($symbol);

      // Parse yahoo object.
      $startDate = DateFormat (ValueGetCritical ($yahooMeta, YM_START, false));
      $endDate = DateFormat (ValueGetCritical ($yahooMeta, YM_END, false));
      $isAlive = $this->IsAlive ($endDate);
      $hasDividends = $this->HasDividends ($symbol, $startDate);

      $this->SecurityDAL->Meta->Insert ($stockId, $startDate, $isAlive, $hasDividends);
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
<?php // Stupid lil' tests
  $sec = new Security ();
  $sec->Test ();
?>
