<?php // Includes
  require_once ("core.php");
?>
<?php // Definitions 
  define ("YAHOO_API_URL", "https://query.yahooapis.com/v1/public");

  define ("YAHOO_CONNECTION_EXCEPTION", "Failed to connect to yahoo api.");
  define ("YAHOO_API_EXCEPTION", "Yahoo API had an internal failure.");
  define ("YAHOO_UNKNOWN_EXCEPTION", "An unknown yahoo API Error occured.");

  define ("DEFAULT_MAX_RETRIES", 20);
?>
<?php // Globals
    $LastYahooError = NULL;
    $CriticalErrors = array (YAHOO_CONNECTION_EXCEPTION, YAHOO_UNKNOWN_EXCEPTION);
    $MaxRetries = DEFAULT_MAX_RETRIES;
?>
<?php
  function YahooEscape ($contents)
  {
    return urlencode ($contents);
  }

  function GetYahooMaxRetries ()
  {
    global $MaxRetries;
    return $MaxRetries;
  }

  function SetYahooMaxRetries ($max)
  {
    global $MaxRetries;
    ExpectId ($max);

    $MaxRetries = $max;
  }

  function BuildYahooQuery ($search, $format, $env, $diag = true)
  {
    $errMsg = "Must provide a %s parameter for the query string.";

    Expect ($search, sprintf ($errMsg, "search"));
    Expect ($format, sprintf ($errMsg, "format"));
    Expect ($env, sprintf ($errMsg, "env"));

    return sprintf ("%s/yql?q=%s&env=%s&format=%s&diagnostics=%s", YAHOO_API_URL, YahooEscape ($search), YahooEscape ($env), YahooEscape ($format), YahooEscape ($diag ? "true" : "false"));
  }

  function ExecYahooQuery ($queryStr, $expectedResults)
  {
    usleep (500000);
    $response = file_get_contents ($queryStr);
    
    return ParseYahooQuery ($response, $expectedResults);
  }

  function RunYahooQuery ($queryStr, $expectedResults)
  {
    // Yahoo API is not reliable.  A great deal of error handling and retrying must be done.
    Debug (sprintf ("Running Query String: %s", $queryStr));
    $maxRetries = GetYahooMaxRetries ();

    $attempts = 0;
    while (!($result = ExecYahooQuery ($queryStr, $expectedResults)) && $attempts < $maxRetries)
    {
      Warn (sprintf ("Yahoo query failed attempt: (%d of %d).  Reason: %s", $attempts+1, $maxRetries, GetYahooError ()));
      ++$attempts;
    }
  
    return $result;
  }

  function ParseYahooQuery ($result, $expectedResults)
  {
    if (is_bool ($result) && !$result)
      return SetYahooError (YAHOO_CONNECTION_EXCEPTION);
    
    if (!is_string ($result) || $result == "")
      return SetYahooError (YAHOO_UNKNOWN_EXCEPTION);

    $jsonResponse = json_decode ($result, true);
    $results = GetObjectArray ($jsonResponse["query"]["results"]);
    if (!ValidResults ($results, $expectedResults))
      return SetYahooError (YAHOO_API_EXCEPTION);

    return $results;
  }

  function SetYahooError ($message)
  {
    global $LastYahooError;
    $LastYahooError = $message;
    return false;
  }

  function GetYahooError ()
  {
    global $LastYahooError;
    return $LastYahooError;
  }

  function ValidResults ($results, $expectedResults)
  {
    if ($expectedResults != NULL)
      foreach ($results as $result)
        foreach ($expectedResults as $expectedResult)
          if (!isset ($result[$expectedResult]))
            return false;

    return true;
  }

  function YahooQuery ($search, $expectedFields, $format = "json", $env = "store://datatables.org/alltableswithkeys")
  {
    $queryStr = BuildYahooQuery ($search, $format, $env);
    $result = RunYahooQuery ($queryStr, $expectedFields);
    
    return CheckCritical ($result);
  }

  function YahooSelect ($from, $where = "", $content = "*", $expectedFields = NULL)
  {
    $whereClause = IsNullOrEmpty ($where)
      ? ""
      : sprintf (" WHERE %s", $where);

    $search = sprintf ("SELECT %s FROM %s%s", $content, $from, $whereClause);
    return YahooQuery ($search, $expectedFields);
  }

  function CheckCritical ($result)
  {
    global $CriticalErrors;
    
    if (is_bool ($result) && !$result)
    {
      $lastError = GetYahooError ();
      foreach ($CriticalErrors as $criticalError)
        if ($lastError == $criticalError)
          throw new Exception ($lastError);
    }

    return $result;
  }
?>
