<?php
  $filenamePre = $argv[1];
  $stepLen = $argv[2];
  $stepNum = $argv[3]; 
  $end = $argv[4];
  $netfilename = $argv[5];
  $loopNum = $argv[6];
  $directory = $argv[7];

  $datafile = $filenamePre . "_data";
  $outputdir = $filenamePre . "_results";
  $subfp = fopen($filenamePre . "_qsubscript", "w");
  $dataprocessfp = fopen($filenamePre . "_dataprocess", "w");
  fprintf($dataprocessfp, "#!/bin/bash\n");
  $namePost = 0;
  while($namePost < $end) {
    $filename = $directory. '/' . $filenamePre . $namePost;
    $fp = fopen($filename, "w") or die("couldn't open $filename");
    fprintf($fp, "#!/bin/bash\n");
    fprintf($fp, "./sourcecode/run $netfilename $loopNum $namePost $stepLen $stepNum | tee $outputdir/$filenamePre$namePost\n");
    fclose($fp);
    fprintf($subfp, "qsub -cwd ./$filename \n");
    if ($namePost == 0) {
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep hybrid > $outputdir/$datafile\n");
    }
    else {
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep hybrid >> $outputdir/$datafile\n");
    }
    $namePost += $stepLen*$stepNum;
  }
  fclose($subfp);

  fprintf($dataprocessfp, "gnuplot << EOF

set terminal postscript eps

set xrange [0:1]
set xlabel 'lambda'
set ylabel 'top L=50 score average'
set output '$outputdir/averagescore_hybrid_$filenamePre.eps'
plot '$outputdir/$datafile' using 3:17 w lp pt 19 ps 0.2 lc 3 title 'averagescore_hybrid'
set output

set xlabel 'lambda'
set ylabel 'RankScore'
set output '$outputdir/RankScore_hybrid_$filenamePre.eps'
plot '$outputdir/$datafile' using 3:7 w lp pt 19 ps 0.2 lc 3 title 'RankScore_hybrid'
set output

set xlabel 'lambda'
set ylabel 'Precision'
set output '$outputdir/Precision_hybrid_$filenamePre.eps'
plot '$outputdir/$datafile' using 3:9 w lp pt 19 ps 0.2 lc 3 title 'Precision_hybrid'
set output

set xlabel 'lambda'
set ylabel 'Intrasimilarity'
set output '$outputdir/Intrasimilarity_hybrid_$filenamePre.eps'
plot '$outputdir/$datafile' using 3:11 w lp pt 19 ps 0.2 lc 3 title 'Intrasimilarity_hybrid'
set output

set xlabel 'lambda'
set ylabel 'HammingD'
set output '$outputdir/HammingD_hybrid_$filenamePre.eps'
plot '$outputdir/$datafile' using 3:13 w lp pt 19 ps 0.2 lc 3 title 'HammingD_hybrid'
set output

set xlabel 'lambda'
set ylabel 'Popularity'
set output '$outputdir/Popularity_hybrid_$filenamePre.eps'
plot '$outputdir/$datafile' using 3:15 w lp pt 19 ps 0.2 lc 3 title 'Popularity_hybrid'
set output

EOF
");

  fclose($dataprocessfp);

  //php generateScripts.php movielens 0.01 5 1.01 data/movielens/movielens_3c 1000 scripts_movielens
  //php generateScripts.php netflix 0.01 5 1.01 data/netflix/netflix_3c 200 scripts_netflix
  //php generateScripts.php rym 0.01 5 1.01 data/rym/rym_3c 50 scripts_rym
  

?>
