<?php

ini_set("memory_limit",-1);

$file = $argv[1];

$fp1 = fopen($file, "r") or die("can not open $file");
//$fpw = fopen("output", "w") or die("can not open output");
//$fpw1 = fopen("output1", "w") or die("can not open output1");

$col2 = array();
$col3 = array();
$col4 = array();
$col10 = array();

$linesNum = 0;
while (!feof($fp1)) {
  $linesNum++;
  $line = fgets($fp1, 10000);
  $line = trim($line);
  $tmp = explode("\t", $line);
	if (count($tmp) != 10) {
		echo "newfile error: linesNum: $linesNum: $line\n";
		continue;
	}

  $tmp[0] = trim($tmp[0]);
  $tmp[1] = trim($tmp[1]);
  $tmp[2] = trim($tmp[2]);
  $tmp[3] = trim($tmp[3]);
  $tmp[4] = trim($tmp[4]);
  $tmp[5] = trim($tmp[5]);
  $tmp[6] = trim($tmp[6]);
  $tmp[7] = trim($tmp[7]);
  $tmp[8] = trim($tmp[8]);
  $tmp[9] = trim($tmp[9]);

  $col2[] = $tmp[1];
  $col3[] = $tmp[2];
  $col4[] = $tmp[3];
  $col10[] = $tmp[9];
  
}

$col = $col4;
$count = count($col);
$total = 0.0;
foreach ($col as $value) {
  $total += $value;
}
$average = $total/$count;
$total = 0.0;
foreach ($col as $value) {
  $total += pow($value/$average - 1, 2);
}
$variance = $total/$count;
echo "col4: average: $average, variance: $variance\n";


$col = $col10;
$count = count($col);
$total = 0.0;
foreach ($col as $value) {
  $total += $value;
}
$average = $total/$count;
$total = 0.0;
foreach ($col as $value) {
  $total += pow($value/$average - 1, 2);
}
$variance = $total/$count;
echo "col10: average: $average, variance: $variance\n";

$col = $col3;
$count = count($col);
$total = 0.0;
foreach ($col as $value) {
  $total += $value;
}
$average = $total/$count;
$total = 0.0;
foreach ($col as $value) {
  $total += pow($value/$average - 1, 2);
}
$variance = $total/$count;
echo "col3: average: $average, variance: $variance\n";

fclose($fp1);
//fclose($fpw);
//fclose($fpw1);
