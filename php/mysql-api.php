<?php // Includes
  require_once ("core.php");
  require_once ("credentials.php")
?>
<?php // Variables
  
?>
<?php // SQL Functions
  function TableFormat ($tableName, $database = "wbs7")
  {
    return sprintf ("[%s].[dbo].[%s]", $database, $tableName);
  }

  function SINGLE ($results)
  {
    return (count ($results) > 0)
      ? $results[0]
      : false;
  }

  function SINGLE_OR_DIE ($results, $errorMessage)
  {
    if (!($single = SINGLE ($results)))
      Error ($errorMessage);

    return $single;
  }

  function SELECT ($contents, $from, $where = "", $joinFrom = "", $joinOn = "")
  {
    Expect ($from, "Must provide a table to select from.");
    $selectStatement = sprintf ("SELECT %s FROM %s", $contents, TableFormat ($from));

    if (!IsNullOrEmpty ($joinFrom))
    {
      Expect ($joinOn, "Must provide a value to join on.");
      $selectStatement = sprintf ("%s JOIN %s ON %s.%s = %s.%s",
        $selectStatement, TableFormat ($joinFrom), $from, $joinOn, $joinFrom, $joinOn);
    }
    
    if (!IsNullOrEmpty ($where))
      $selectStatement = sprintf ("%s WHERE %s", $selectStatement, $where);

    return SqlCmd ($selectStatement);
  }

  function UPDATE ($from, $modify, $where)
  {
    Expect ($where, "Must provide a where clause for updating.");
    Expect ($modify, "Must provide values to be updated.");
    Expect ($from, "Must provide a table to update.");
    
    $updateStatement = sprintf ("UPDATE %s SET %s WHERE %s", TableFormat ($from), $modify, $where);
    return SqlCmd ($updateStatement);
  }

  function INSERT ($from, $columns, $values)
  {
    Expect ($from, "Must provide a table to insert into.");
    Expect ($columns, "Must provide columns for inserted values.");
    Expect ($values, "Must provide values for insertion.");

    $insertStatement = sprintf ("INSERT INTO %s %s VALUES %s", TableFormat ($from), $columns, $values);
    return SqlCmd ($insertStatement);
  }

  function DELETE ($from, $where)
  {
    Expect ($from, "Must provide a table to delete from.");
    Expect ($where, "Must provide a condition for deletion.");

    $deleteStatement = sprintf ("DELETE FROM %s WHERE %s", TableFormat ($from), $where);
    return SqlCmd ($deleteStatement);
  }

  function SqlCmd ($cmd)
  {
    global $sqlServer, $sqlUser, $sqlPass;
    $sqlConnection = mssql_connect ($sqlServer, $sqlUser, $sqlPass);
    if (!$sqlConnection)
      die("Couldn't connect to server.");

    Debug (sprintf ("Sql Command: %s", $cmd));
    $result = array();
    $query = mssql_query ($cmd);
    
    if (!is_bool ($query) && mssql_num_rows ($query) > 0)
    {
      while ($row = mssql_fetch_array ($query))
        array_push ($result, $row);

      mssql_free_result ($query);
    }

    mssql_close($sqlConnection);
    return $result;
  }
?>
