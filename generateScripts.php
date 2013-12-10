<?php
  $filenamePre = $argv[1];
  $stepLen = $argv[2];
  $stepNum = $argv[3]; 
  $end = $argv[4];
  $netfilename = $argv[5];
  $loopNum = $argv[6];
  $directory = $argv[7];

  $subfp = fopen($filenamePre . "_qsubscript", "w");
  $namePost = 0;
  while($namePost < $end) {
    $filename = $directory . '/' . $filenamePre . $namePost;
    $fp = fopen($filename, "w") or die("couldn't open $filename");
    fprintf($fp, "sourcecode/run $netfilename $loopNum $namePost $stepLen $stepNum");
    fclose($fp);
    fprintf($subfp, "qsub -cwd $filename\n");
    $namePost += $stepLen*$stepNum;
  }
  fclose($subfp);

  //php generateScripts.php movielens 0.01 10 1 data/movielen/movielens.txt 100 scripts_movielens
  //php generateScripts.php netflix 0.01 10 1 data/netflix/netflix_with_rating.txt 100 scripts_netflix
  //php generateScripts.php rym 0.01 10 1 data/rym/rym_clean 100 scripts_rym
  

?>
