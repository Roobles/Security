<?php // Functions
  // Validation Methods
  function IsNullOrEmpty ($value)
  {
    return (!isset ($value) || trim ($value) === '');
  }

  // Basic I/O Methods
  function Display ($header, $message, $color, $error = false)
  {
    $output = sprintf ("\e[%sm%s\e[0m: %s\n", $color, $header, $message);
    $file = $error ? STDERR : STDOUT;
    fwrite ($file, $output);
  }

  function Debug ($message)
  {
    global $debug;

    if ($debug)
      Display ("DEBUG", $message, "1;33");
  }

  function Error ($message, $code = 1)
  {
    Display ("ERROR", $message, "1;31", true);
    exit ($code);
  }

  function Warn ($message)
  {
    Display ("WARNING", $message, "1;31");
  }

  function Message ($message)
  {
    Display ("MESSAGE", $message, "0;32");
  }

  // Error Handling Methods
  function Expect ($value, $message, $code = 1)
  {
    if (IsNullOrEmpty ($value))
      Error ($message, $code);
  }
    
  function ExpectId ($value, $code = 1)
  {
    if (!is_numeric ($value) || intval ($value) < 1)
      Error (sprintf ("'%s' is not a valid Id.", $value), $code);
  }

  function ExpectUint ($value, $code = 1)
  {
    if (!is_numeric ($value) || intval ($value) < 0)
      Error (sprintf ("'%s' is not a valid unsigned int.", $value, $code));
  }

  function ExpectDate ($value, $code = 1)
  {
    if (!strtotime ($value))
      Error (sprintf ("'%s' is not a valid date.", $value), $code);
  }
  
  function ExpectBool ($value, $code = 1)
  {
    if (!is_bool ($value))
      Error ("Expected a type of boolean.", $code);
  }

  function ExpectNumeric ($value, $code = 1)
  {
    if (!is_numeric ($value))
      Error ("Expected a numeric type.", $code);
  }

  function ExpectObject ($value, $code = 1)
  {
    if (!is_object ($value))
      Error ("Expected an object type.", $code);
  }

  function ExpectArray ($value, $code = 1)
  {
    if (!is_array ($value))
      Error ("Expected an array type.", $code);
  }

  // Date Methods
  function DateGet ($dateObj = NULL)
  {
    if ($dateObj == NULL)
      $dateObj = "now";

    if  (is_object ($dateObj))
      return $dateObj;

    if (is_string ($dateObj))
      return new DateTime (DateSanitize ($dateObj));
  }

  function DateSanitize ($dateStr)
  {
    return str_replace ("NaN", "01", $dateStr);
  }

  function DateGetToday ()
  {
    return DateGet ("now");
  }

  function DateFormat ($date = "")
  {
    $date = DateGet ($date);
    $dateFormat = "Y-m-d";
    return $date->format ($dateFormat);
  }

  function DateSqlFormat ($date = NULL)
  {
    return $date == NULL 
      ? "NULL"
      : sprintf ("'%s'", DateFormat ($date));
  }

  function DateCompare ($firstDate, $secondDate)
  {
    $firstVal = strtotime (DateFormat ($firstDate));
    $secondVal = strtotime (DateFormat ($secondDate));

    if ($firstVal == $secondVal)
      return 0;

    return $firstVal > $secondVal
      ? 1
      : -1;
  }

  // Boolean Methods
  function BoolFormat ($bool)
  {
    return !$bool ? 0 : 1;
  }

  // Retrieval Methods
  function ValueGet ($array, $key)
  {
    ExpectArray ($array); 
    if (!isset ($array[$key]))
      return NULL;

    return $array[$key];
  }

  function ValueGetCritical ($array, $key, $fatal = true)
  {
    $value = ValueGet ($array, $key);

    if ($value == NULL) 
    {
      $errMessage = sprintf ("Critical value '%s' was not found in array.", $key);
      if (!$fatal) throw new Exception ($errMessage);
      Error ($errMessage);
    }

    return $value;
  }

  function Single ($results)
  {
    if (!is_array ($results))
      return false;

    return (count ($results) > 0)
      ? reset ($results)
      : false;
  }

  function StepMultiDimensional ($array)
  {
    if (!is_array ($array))
      return false;

    $firstChild = reset ($array);
    if (is_array ($firstChild))
      if (is_array (reset ($firstChild)))
        return $firstChild;

    return false;
  }


  // Object Printing Methods
  function SanitizeObjectValue ($objectValue)
  {
    $sanitized = strtr ($objectValue, hex2bin ("a0"), ' ');
    return strtr (trim ($sanitized), ' ', '_');
  }

  function PrintObjectValue ($objectKey, $objectValue, $strTerminal)
  {
    printf ("%s:%s%s", $objectKey, SanitizeObjectValue ($objectValue), $strTerminal);
  }

  function PrintObject ($object, $divider = 1, $keyIndex = 0)
  {
    if (!$object) return;
    if (!is_array ($object)) $object = array("Value" => $object);

    $objectNum = count ($object) / $divider;
    $objectKeys = array_keys ($object);
    $objectValues = array_values ($object);

    for ($i = 0; $i < $objectNum; $i++)
    {
      $objectKey = $objectKeys[($i * $divider) + $keyIndex];
      $objectVal = $objectValues[$i];
      $strTerminal = $i == ($objectNum - 1) ? "\n" : " ";
      PrintObjectValue ($objectKey, $objectVal, $strTerminal);
    }
  }

  function GetObjectArray ($objects)
  {
    if (!is_array ($objects))
      return array ($objects);

    if ($child = StepMultiDimensional ($objects))
      return GetObjectArray ($child);

    return $objects;
  }

  function PrintObjects ($objects, $divider = 1, $keyIndex = 0)
  {
    if (!$objects) return;
    $objectArray = GetObjectArray ($objects);
    foreach ($objectArray as $object)
      PrintObject ($object, $divider, $keyIndex);
  }
?>
