<?php
  $filenamePre = $argv[1];
  $stepLen = $argv[2];
  $stepNum = $argv[3]; 
  $end = $argv[4];
  $netfilename = $argv[5];
  $loopNum = $argv[6];
  $directory = $argv[7];
  $maxscore = $argv[8];

  $datafile_score = $filenamePre . "_score_data";
  $datafile_degree = $filenamePre . "_degree_data";
  $datafile_third = $filenamePre . "_third_data";
  $outputdir = $filenamePre . "_results";
  $subfp = fopen($filenamePre . "_qsubscript", "w");
  $dataprocessfp = fopen($filenamePre . "_dataprocess", "w");
  fprintf($dataprocessfp, "#!/bin/bash\n");
  $namePost = 0;
  while($namePost < $end) {
    $filename = $directory. '/' . $filenamePre . $namePost;
    $fp = fopen($filename, "w") or die("couldn't open $filename");
    fprintf($fp, "#!/bin/bash\n");
    fprintf($fp, "./sourcecode/run $netfilename $maxscore $loopNum $namePost $stepLen $stepNum | tee $outputdir/$filenamePre$namePost\n");
    fclose($fp);
    fprintf($subfp, "qsub -cwd ./$filename \n");
    if ($namePost == 0) {
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep score > $outputdir/$datafile_score\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep degree > $outputdir/$datafile_degree\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep third > $outputdir/$datafile_third\n");
    }
    else {
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep score >> $outputdir/$datafile_score\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep degree >> $outputdir/$datafile_degree\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep third >> $outputdir/$datafile_third\n");
    }
    $namePost += $stepLen*$stepNum;
  }
  fclose($subfp);

  fprintf($dataprocessfp, "gnuplot << EOF

set terminal postscript eps

set xlabel 'theta'
set ylabel 'top L=50 score average'
set output '$outputdir/averagescore_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:17 w lp pt 19 ps 0.2 lc 3 title 'averagescore_score', '$outputdir/$datafile_degree' using 3:17 w lp pt 19 ps 0.2 lc 4 title 'averagescore_degree', '$outputdir/$datafile_third' using 3:17 w lp pt 19 ps 0.2 lc 1 title 'averagescore_third'
set output

set xlabel 'theta'
set ylabel 'RankScore'
set output '$outputdir/RankScore_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:7 w lp pt 19 ps 0.2 lc 3 title 'RankScore_score', '$outputdir/$datafile_degree' using 3:7 w lp pt 19 ps 0.2 lc 4 title 'RankScore_degree', '$outputdir/$datafile_third' using 3:7 w lp pt 19 ps 0.2 lc 1 title 'RankScore_third'
set output

set xlabel 'theta'
set ylabel 'Precision'
set output '$outputdir/Precision_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:9 w lp pt 19 ps 0.2 lc 3 title 'Precision_score', '$outputdir/$datafile_degree' using 3:9 w lp pt 19 ps 0.2 lc 4 title 'Precision_degree', '$outputdir/$datafile_third' using 3:9 w lp pt 19 ps 0.2 lc 1 title 'Precision_third'
set output

set xlabel 'theta'
set ylabel 'Intrasimilarity'
set output '$outputdir/Intrasimilarity_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:11 w lp pt 19 ps 0.2 lc 3 title 'Intrasimilarity_score', '$outputdir/$datafile_degree' using 3:11 w lp pt 19 ps 0.2 lc 4 title 'Intrasimilarity_degree', '$outputdir/$datafile_third' using 3:11 w lp pt 19 ps 0.2 lc 1 title 'Intrasimilarity_third'
set output

set xlabel 'theta'
set ylabel 'HammingD'
set output '$outputdir/HammingD_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:13 w lp pt 19 ps 0.2 lc 3 title 'HammingD_score', '$outputdir/$datafile_degree' using 3:13 w lp pt 19 ps 0.2 lc 4 title 'HammingD_degree', '$outputdir/$datafile_third' using 3:13 w lp pt 19 ps 0.2 lc 1 title 'HammingD_third'
set output

set xlabel 'theta'
set ylabel 'Popularity'
set output '$outputdir/Popularity_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:15 w lp pt 19 ps 0.2 lc 3 title 'Popularity_score', '$outputdir/$datafile_degree' using 3:15 w lp pt 19 ps 0.2 lc 4 title 'Popularity_degree', '$outputdir/$datafile_third' using 3:15 w lp pt 19 ps 0.2 lc 1 title 'Popularity_third'
set output

EOF
");

  fclose($dataprocessfp);

  //php generateScripts.php movielens 0.01 5 1.01 data/movielen/movielens.txt 100 scripts_movielens 5
  //php generateScripts.php netflix 0.01 5 1.01 data/netflix/netflix_with_rating.txt 100 scripts_netflix 5
  //php generateScripts.php rym 0.01 5 1.01 data/rym/rym_clean 100 scripts_rym 10
  

?>
