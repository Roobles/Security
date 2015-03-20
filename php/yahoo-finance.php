<?php // Includes
  require_once ("yahoo-api.php");
  require_once ("core.php");
?>
<?php // Definitions
  define ("HISTORICAL", "Historical");
  define ("DIVIDENDS", "Dividends");
  define ("METADATA", "Metadata");

  define ("YM_SYMBOL", "symbol");
  define ("YM_START", "start");
  define ("YM_END", "end");

  define ("YH_SYMBOL", "Symbol");
  define ("YH_DATE", "Date");
  define ("YH_HIGH", "High");
  define ("YH_LOW", "Low");
  define ("YH_CLOSE", "Close");
?>
<?php // Class
  class YahooFinance
  {
    // Private Members
    private $FinanceLibraries = array (
      HISTORICAL => "yahoo.finance.historicaldata",
      DIVIDENDS => "yahoo.finance.dividendhistory",
      METADATA => "yahoo.finance.stocks"
    );

    // Public Methods
    public function GetMetaData ($symbol)
    {
      Expect ($symbol, "Must provide symbol to look up meta date for.");
  
      $contents = sprintf ("%s,%s,%s", YM_SYMBOL, YM_START, YM_END);
      $from = $this->FinanceLibraries[METADATA];
      $where = sprintf ("%s = \"%s\"", YM_SYMBOL, $symbol);
  
      $response = YahooSelect ($from, $where, $contents);
      return Single ($response);
    }

    public function GetStockHistory ($symbol, $startDate, $endDate = NULL)
    {
      $contents = sprintf ("%s,%s,%s,%s,%s", YH_SYMBOL, YH_DATE, YH_HIGH, YH_LOW, YH_CLOSE);
      $from = $this->FinanceLibraries[HISTORICAL];

      return $this->GetHistory ($symbol, $startDate, $endDate, $contents, $from);
    }

    public function GetDividendHistory ($symbol, $startDate, $endDate = NULL)
    {
      $contents = "*";
      $from = $this->FinanceLibraries[DIVIDENDS];

      return $this->GetHistory ($symbol, $startDate, $endDate, $contents, $from);
    }

    // Private Methods
    private function GetHistory ($symbol, $startDate, $endDate, $contents, $from)
    {
      Expect ($symbol, "Must provide a stock symbol to look up.");
      Expect ($startDate, "Must provide a start date.");
  
      $startDate = DateGet ($startDate);
      $endDate = DateGet ($endDate);
  
      if (DateCompare ($endDate, $startDate) < 0)
        Error ("Attempted to retrieve history with a start date later than the end date.");
   
      $where = sprintf ("symbol = \"%s\" AND startDate = \"%s\" AND endDate = \"%s\"", $symbol, DateFormat ($startDate), DateFormat ($endDate));

      $response = YahooSelect ($from, $where, $contents);   
      return $response;
    }
  }
?>
