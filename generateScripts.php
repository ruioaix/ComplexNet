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

  if ($filenamePre == 'netflix') {
    $best_lambda = 0.17;
    $best_rank = 0.060601;
    $best_precision = 0.067722;
    $best_intrasimilarity = 0.0339877;
    $best_hammingd = 0.555959;
    $best_popularity = 827.64;
    $best_score = 3.66041;
  }
  else if ($filenamePre == 'rym') {
    $best_lambda = 0.23;
    $best_rank = 0.057259;
    $best_precision = 0.040551;
    $best_intrasimilarity = 0.126140;
    $best_hammingd = 0.923887;
    $best_popularity = 608.25;
    $best_score = 7.559685;
  }
  else {
  // ($filenamePre == 'movielens') {
    $best_lambda = 0.15;
    $best_rank = 0.092687;
    $best_precision = 0.098539;
    $best_intrasimilarity = 0.319445;
    $best_hammingd = 0.842276;
    $best_popularity = 175.56;
    $best_score = 3.596291;
  }

  fprintf($dataprocessfp, "gnuplot << EOF

set terminal postscript eps

set xlabel 'theta'
set ylabel 'top L=50 score average'
set output '$outputdir/averagescore_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:17 w lp pt 19 ps 0.2 lc 3 title 'averagescore_score', '$outputdir/$datafile_degree' using 3:17 w lp pt 19 ps 0.2 lc 4 title 'averagescore_degree', '$outputdir/$datafile_third' using 3:17 w lp pt 19 ps 0.2 lc 1 title 'averagescore_third', $best_score with dots
set output

set xlabel 'theta'
set ylabel 'RankScore'
set output '$outputdir/RankScore_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:7 w lp pt 19 ps 0.2 lc 3 title 'RankScore_score', '$outputdir/$datafile_degree' using 3:7 w lp pt 19 ps 0.2 lc 4 title 'RankScore_degree', '$outputdir/$datafile_third' using 3:7 w lp pt 19 ps 0.2 lc 1 title 'RankScore_third', $best_rank with dots
set output

set xlabel 'theta'
set ylabel 'Precision'
set output '$outputdir/Precision_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:9 w lp pt 19 ps 0.2 lc 3 title 'Precision_score', '$outputdir/$datafile_degree' using 3:9 w lp pt 19 ps 0.2 lc 4 title 'Precision_degree', '$outputdir/$datafile_third' using 3:9 w lp pt 19 ps 0.2 lc 1 title 'Precision_third', $best_precision with dots
set output

set xlabel 'theta'
set ylabel 'Intrasimilarity'
set output '$outputdir/Intrasimilarity_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:11 w lp pt 19 ps 0.2 lc 3 title 'Intrasimilarity_score', '$outputdir/$datafile_degree' using 3:11 w lp pt 19 ps 0.2 lc 4 title 'Intrasimilarity_degree', '$outputdir/$datafile_third' using 3:11 w lp pt 19 ps 0.2 lc 1 title 'Intrasimilarity_third', $best_intrasimilarity with dots
set output

set xlabel 'theta'
set ylabel 'HammingD'
set output '$outputdir/HammingD_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:13 w lp pt 19 ps 0.2 lc 3 title 'HammingD_score', '$outputdir/$datafile_degree' using 3:13 w lp pt 19 ps 0.2 lc 4 title 'HammingD_degree', '$outputdir/$datafile_third' using 3:13 w lp pt 19 ps 0.2 lc 1 title 'HammingD_third', $best_hammingd with dots
set output

set xlabel 'theta'
set ylabel 'Popularity'
set output '$outputdir/Popularity_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:15 w lp pt 19 ps 0.2 lc 3 title 'Popularity_score', '$outputdir/$datafile_degree' using 3:15 w lp pt 19 ps 0.2 lc 4 title 'Popularity_degree', '$outputdir/$datafile_third' using 3:15 w lp pt 19 ps 0.2 lc 1 title 'Popularity_third', $best_popularity with dots
set output

EOF
");

  fclose($dataprocessfp);

  //php generateScripts.php movielens 0.02 5 2.01 data/movielen/movielens.txt 100 scripts_movielens 5
  //php generateScripts.php netflix 0.02 5 2.01 data/netflix/netflix_with_rating.txt 100 scripts_netflix 5
  //php generateScripts.php rym 0.02 5 2.01 data/rym/rym_clean 10 scripts_rym 10
  //php generateScripts.php rymA 0.02 5 2.01 data/rym/rym_clean 10 scripts_rymA 10
  

?>
