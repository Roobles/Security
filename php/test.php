<?php // Includes
  // Just add the domain for now.  It will include everything.
  require_once ("domain-security.php");
?>
<?php // MAIN
  $debug = false;
  /*
  // Yahoo Tests
  $goodTicker = "0029.KL";
  $badTicker = "0035@BN.KL";
  $apple = "aapl";

  $ticker = $apple;
  $yf = new YahooFinance ();

  $result = $yf->GetDividendHistory ($ticker, "3/10/2014");
  PrintObjects ($result);
  //*/

  //*
  // Domain Tests
  $sec = new Security ();
  $sec->Test ();
  //*/
?>
