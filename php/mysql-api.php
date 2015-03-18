<?php // Includes
  require_once ("core.php");
  require_once ("credentials.php")
?>
<?php // Variables
  $dbSecurity = NULL;
  $dbIsOpen = false;
?>
<?php // SQL Functions
  function TableFormat ($tableName, $database = "wbs7")
  {
    return sprintf ("%s", $tableName);
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
    global $dbSecurity, $dbIsOpen;
    if (!$dbIsOpen) SqlOpen ();
    Debug (sprintf ("Sql Command: %s", $cmd));
    $result = array ();
    $query = $dbSecurity->query ($cmd);
    
    if (!is_bool ($query) && $query->num_rows > 0)
    {
      while ($row = $query->fetch_assoc ())
        array_push ($result, $row);

      $query->free ();
    }

    return $result;
  }

  function SqlOpen ()
  {
    global $dbSecurity, $dbIsOpen, $mysqlUser, $mysqlPass, $mysqlServer, $mysqlDatabase;

    Debug ("Opening connection to security database.");
    if ($dbIsOpen && $dbSecurity != NULL) 
    {
      Debug ("Connection already open; silently returning.");
      return;
    }

    $dbSecurity = new mysqli ($mysqlServer, $mysqlUser, $mysqlPass, $mysqlDatabase);
    if ($dbSecurity->connect_errno) Error ("Could not connect to the security database.");

    $dbIsOpen = true;
  }

  function SqlClose ()
  {
    global $dbSecurity,$dbIsOpen;
    Debug ("Closing connection to security database.");
    if (!$dbIsOpen && $dbSecurity == NULL)
    {
      Debug ("Connection already closed; silently returning.");
      return;
    }

    $dbSecurity->close ();
    $dbSecurity = NULL;
    $dbIsOpen = false;
  }
?>
