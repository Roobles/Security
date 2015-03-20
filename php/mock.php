<?php // Includes
  require_once ("core.php");
?>
<?php // Class
  class MockYahooFinance
  {
    // Public Methods
    public function GetMetaData ($symbol)
    {
      $start = DateFormat ("2/15/1989");
      $end = DateFormat (DateGetToday());

      return array ("symbol" => $symbol, "start" => $start, "end" => $end);
    }

    public function GetStockHistory ($symbol, $startDate, $endDate = NULL)
    {
    }

    public function GetDividendHistory ($symbol, $startDate, $endDate = NULL)
    {
      // TODO: Do a better implementation of this function.
      $dividends = array (
        array ("foo" => 1, "bar" => 2),
      );

      return $dividends;
    }

    // Private Methods
    private function GetHistory ($symbol, $startDate, $endDate, $contents, $from)
    {
    }
  }
?>
