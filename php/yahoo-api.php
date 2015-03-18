<?php // Includes
  require_once ("core.php");
?>
<?php // Definitions 
  define ("YAHOO_API_URL", "https://query.yahooapis.com/v1/public");
?>
<?php // Environment Variables
  // Yahoo Finance Libraries
  $yfl = array (
    "Historical" => "yahoo.finance.historicaldata",
    "Dividends" => "yahoo.finance.dividendhistory",
    "Metadata" => "yahoo.finance.stocks"
  );
?>
<?php
  function YahooEscape ($contents)
  {
    return urlencode ($contents);
  }

  function BuildYahooQuery ($search, $format, $env)
  {
    $errMsg = "Must provide a %s parameter for the query string.";

    Expect ($search, sprintf ($errMsg, "search"));
    Expect ($format, sprintf ($errMsg, "format"));
    Expect ($env, sprintf ($errMsg, "env"));

    return sprintf ("%s/yql?q=%s&env=%s&format=%s", YAHOO_API_URL, YahooEscape ($search), YahooEscape ($env), YahooEscape ($format));
  }

  function RunYahooQuery ($queryStr)
  {
    Debug (sprintf ("Running Query String: %s", $queryStr));
    $result = file_get_contents ($queryStr);
    return $result 
      ? $result
      : "";
  }

  function ParseYahooQuery ($result)
  {
    Expect ($result, "Invalid result from yahoo query.");
    $jsonResponse = json_decode ($result, true);

    return $jsonResponse["query"]["results"];
  }

  function YahooQuery ($search, $format = "json", $env = "store://datatables.org/alltableswithkeys")
  {
    $queryStr = BuildYahooQuery ($search, $format, $env);
    $result = RunYahooQuery ($queryStr);

    return ParseYahooQuery ($result);
  }

  function YahooSelect ($from, $where = "", $content = "*")
  {
    $whereClause = IsNullOrEmpty ($where)
      ? ""
      : sprintf (" WHERE %s", $where);

    $search = sprintf ("SELECT %s FROM %s%s", $content, $from, $whereClause);
    return YahooQuery ($search);
  }

  // TODO: Get rid of this stupid little test main.
  $from = $yfl["Metadata"];
  $content = "*";
  $where = "symbol in (\"aapl\", \"fmc\")";

  $debug = true;
  $hist_data = YahooSelect ($from, $where, $content);

  PrintObjects ($hist_data);
?>
