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

  // Date Methods
  function DateGet ($dateObj = NULL)
  {
    if ($dateObj == NULL)
      $dateObj = "now";

    if  (is_object ($dateObj))
      return $dateObj;

    if (is_string ($dateObj))
      return new DateTime ($dateObj);
  }

  function DateGetToday ()
  {
    return GetDate ("now");
  }

  function DateFormat ($date)
  {
    $dateFormat = "Y-m-d";
    return $date->format ($dateFormat);
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

    if (count ($objects) > 1)
      return $objects;

    $firstChild = reset ($objects);

    if (is_array ($firstChild))
      return GetObjectArray ($firstChild);

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
