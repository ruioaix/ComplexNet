<?php
  $filenamePre = $argv[1];
  $stepLen = $argv[2];
  $stepNum = $argv[3]; 
  $end = $argv[4];
  $netfilename = $argv[5];
  $maxscore = $argv[6];
  $loopNum = $argv[7];

  $namePost = 0;
  while($namePost < $end) {
    $filename = "scripts/". $filenamePre . $namePost;
    $fp = fopen($filename, "w") or die("couldn't open $filename");
    fprintf($fp, "sourcecode/run $netfilename $maxscore $loopNum $namePost $stepLen $stepNum");
    fclose($fp);
    $namePost += $stepLen*$stepNum;
  }
  //php generateScripts.php rym 0.02 10 2 data/rym/rym_clean 10 100

?>
