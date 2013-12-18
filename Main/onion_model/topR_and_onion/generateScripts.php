<?php
  $filenamePre = $argv[1];
  $stepLen = $argv[2];
  $topstepLen = $argv[3];
  $stepNum = $argv[4]; 
  $end = $argv[5];
  $netfilename = $argv[6];
  $loopNum = $argv[7];
  $directory = $argv[8];

  $datafile_onion = $filenamePre . "_onion_data";
  $datafile_topR = $filenamePre . "_topR_data";
  $outputdir = $filenamePre . "_results";
  $subfp = fopen($filenamePre . "_qsubscript", "w");
  $dataprocessfp = fopen($filenamePre . "_dataprocess", "w");
  fprintf($dataprocessfp, "#!/bin/bash\n");
  $namePost = 0;
  $topnamePost = 0;
  while($namePost < $end) {
    $filename = $directory. '/' . $filenamePre . $namePost;
    $fp = fopen($filename, "w") or die("couldn't open $filename");
    fprintf($fp, "#!/bin/bash\n");
    fprintf($fp, "./sourcecode/run $netfilename $loopNum $namePost $stepLen $topnamePost $topstepLen $stepNum | tee $outputdir/$filenamePre$namePost\n");
    fclose($fp);
    fprintf($subfp, "qsub -cwd ./$filename \n");
    if ($namePost == 0) {
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep onion > $outputdir/$datafile_onion\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep topR > $outputdir/$datafile_topR\n");
    }
    else {
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep onion >> $outputdir/$datafile_onion\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep topR >> $outputdir/$datafile_topR\n");
    }
    $namePost += $stepLen*$stepNum;
    $topnamePost += $topstepLen*$stepNum;
  }
  fclose($subfp);

  fprintf($dataprocessfp, "gnuplot << EOF

set terminal postscript eps

set xlabel 'orate'
set xtics nomirror
set x2tics
set x2label 'topR'
set ylabel 'RankScore'
set output '$outputdir/RankScore_$filenamePre.eps'
plot '$outputdir/$datafile_onion' using 3:7 w lp pt 19 ps 0.2 lc 3 axis x1y1 title 'RankScore_onion', '$outputdir/$datafile_topR' using 3:7 w lp pt 19 ps 0.2 lc 1 axis x2y1 title 'RankScore_topR'
set output

set xlabel 'orate'
set xtics nomirror
set x2tics
set x2label 'topR'
set ylabel 'Precision'
set output '$outputdir/Precision_$filenamePre.eps'
plot '$outputdir/$datafile_onion' using 3:9 w lp pt 19 ps 0.2 lc 3 axis x1y1 title 'Precision_onion', '$outputdir/$datafile_topR' using 3:9 w lp pt 19 ps 0.2 lc 1 axis x2y1 title 'Precision_topR'
set output

set xlabel 'orate'
set xtics nomirror
set x2tics
set x2label 'topR'
set ylabel 'Intrasimilarity'
set output '$outputdir/Intrasimilarity_$filenamePre.eps'
plot '$outputdir/$datafile_onion' using 3:11 w lp pt 19 ps 0.2 lc 3 axis x1y1 title 'Intrasimilarity_onion', '$outputdir/$datafile_topR' using 3:11 w lp pt 19 ps 0.2 lc 1 axis x2y1 title 'Intrasimilarity_topR'
set output

set xlabel 'orate'
set xtics nomirror
set x2tics
set x2label 'topR'
set ylabel 'HammingD'
set output '$outputdir/HammingD_$filenamePre.eps'
plot '$outputdir/$datafile_onion' using 3:13 w lp pt 19 ps 0.2 lc 3 axis x1y1 title 'HammingD_onion', '$outputdir/$datafile_topR' using 3:13 w lp pt 19 ps 0.2 lc 1 axis x2y1 title 'HammingD_topR'
set output

set xlabel 'orate'
set xtics nomirror
set x2tics
set x2label 'topR'
set ylabel 'Popularity'
set output '$outputdir/Popularity_$filenamePre.eps'
plot '$outputdir/$datafile_onion' using 3:15 w lp pt 19 ps 0.2 lc 3 axis x1y1 title 'Popularity_onion', '$outputdir/$datafile_topR' using 3:15 w lp pt 19 ps 0.2 lc 1 axis x2y1 title 'Popularity_topR'
set output



EOF
");

  fclose($dataprocessfp);

  //php generateScripts.php movielens 0.01 9 5 0.99 data/movielens/movielen2 100 scripts_movielens
  //php generateScripts.php netflix 0.01 30 5 0.99 data/netflix/netflix_pnas.txt 50 scripts_netflix
  //php generateScripts.php rym 0.02 5 2.01 data/rym/rym_clean 10 scripts_rym
  //php generateScripts.php rymA 0.02 5 2.01 data/rym/rym_clean 10 scripts_rymA
  

?>
