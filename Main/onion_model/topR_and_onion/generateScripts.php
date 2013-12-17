<?php
  $filenamePre = $argv[1];
  $stepLen = $argv[2];
  $stepNum = $argv[3]; 
  $end = $argv[4];
  $netfilename = $argv[5];
  $loopNum = $argv[6];
  $directory = $argv[7];

  $datafile_onion = $filenamePre . "_onion_data";
  $datafile_topR = $filenamePre . "_topR_data";
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
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep onion > $outputdir/$datafile_onion\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep topR > $outputdir/$datafile_topR\n");
    }
    else {
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep onion >> $outputdir/$datafile_onion\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep topR >> $outputdir/$datafile_topR\n");
    }
    $namePost += $stepLen*$stepNum;
  }
  fclose($subfp);

  fprintf($dataprocessfp, "gnuplot << EOF

set terminal postscript eps

set xlabel 'orate'
set ylabel 'RankScore'
set output '$outputdir/RankScore_onion_$filenamePre.eps'
plot '$outputdir/$datafile_onion' using 3:7 w lp pt 19 ps 0.2 lc 3 title 'RankScore_score'
set output

set xlabel 'orate'
set ylabel 'Precision'
set output '$outputdir/Precision_onion_$filenamePre.eps'
plot '$outputdir/$datafile_onion' using 3:9 w lp pt 19 ps 0.2 lc 3 title 'Precision_score'
set output

set xlabel 'orate'
set ylabel 'Intrasimilarity'
set output '$outputdir/Intrasimilarity_onion_$filenamePre.eps'
plot '$outputdir/$datafile_onion' using 3:11 w lp pt 19 ps 0.2 lc 3 title 'Intrasimilarity_score'
set output

set xlabel 'orate'
set ylabel 'HammingD'
set output '$outputdir/HammingD_onion_$filenamePre.eps'
plot '$outputdir/$datafile_onion' using 3:13 w lp pt 19 ps 0.2 lc 3 title 'HammingD_score'
set output

set xlabel 'orate'
set ylabel 'Popularity'
set output '$outputdir/Popularity_onion_$filenamePre.eps'
plot '$outputdir/$datafile_onion' using 3:15 w lp pt 19 ps 0.2 lc 3 title 'Popularity_score'
set output

set xlabel 'topR'
set ylabel 'RankScore'
set output '$outputdir/RankScore_topR_$filenamePre.eps'
plot '$outputdir/$datafile_topR' using 3:7 w lp pt 19 ps 0.2 lc 3 title 'RankScore_score'
set output

set xlabel 'topR'
set ylabel 'Precision'
set output '$outputdir/Precision_topR_$filenamePre.eps'
plot '$outputdir/$datafile_topR' using 3:9 w lp pt 19 ps 0.2 lc 3 title 'Precision_score'
set output

set xlabel 'topR'
set ylabel 'Intrasimilarity'
set output '$outputdir/Intrasimilarity_topR_$filenamePre.eps'
plot '$outputdir/$datafile_topR' using 3:11 w lp pt 19 ps 0.2 lc 3 title 'Intrasimilarity_score'
set output

set xlabel 'topR'
set ylabel 'HammingD'
set output '$outputdir/HammingD_topR_$filenamePre.eps'
plot '$outputdir/$datafile_topR' using 3:13 w lp pt 19 ps 0.2 lc 3 title 'HammingD_score'
set output

set xlabel 'topR'
set ylabel 'Popularity'
set output '$outputdir/Popularity_topR_$filenamePre.eps'
plot '$outputdir/$datafile_topR' using 3:15 w lp pt 19 ps 0.2 lc 3 title 'Popularity_score'
set output

EOF
");

  fclose($dataprocessfp);

  //php generateScripts.php movielens 0.01 5 0.99 data/movielen/movielen2 100 scripts_movielens
  //php generateScripts.php netflix 0.02 5 2.01 data/netflix/netflix_pans.txt 100 scripts_netflix
  //php generateScripts.php rym 0.02 5 2.01 data/rym/rym_clean 10 scripts_rym
  //php generateScripts.php rymA 0.02 5 2.01 data/rym/rym_clean 10 scripts_rymA
  

?>
