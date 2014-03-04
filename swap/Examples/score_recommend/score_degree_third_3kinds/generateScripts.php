<?php
  $filenamePre = $argv[1];
  $stepLen = $argv[2];
  $stepNum = $argv[3]; 
  $end = $argv[4];
  $netfilename = $argv[5];
  $loopNum = $argv[6];
  $directory = $argv[7];
  $maxscore = $argv[8];
  $bestlambda = $argv[9];

  $datafile_score = $filenamePre . "_score_data";
  $datafile_degree = $filenamePre . "_degree_data";
  $datafile_third = $filenamePre . "_third_data";
  $datafile_hybrid = $filenamePre . "_hybrid_data";
  $datafile_mass = $filenamePre . "_mass_data";
  $outputdir = $filenamePre . "_results";
  $subfp = fopen($filenamePre . "_qsubscript", "w");
  $dataprocessfp = fopen($filenamePre . "_dataprocess", "w");
  fprintf($dataprocessfp, "#!/bin/bash\n");
  $namePost = 0;
  $mass_hybrid_num = 0;
  while($namePost < $end) {
    $filename = $directory. '/' . $filenamePre . $namePost;
    $fp = fopen($filename, "w") or die("couldn't open $filename");
    fprintf($fp, "#!/bin/bash\n");
    fprintf($fp, "./sourcecode/run $netfilename $maxscore $loopNum $namePost $stepLen $stepNum $bestlambda | tee $outputdir/$filenamePre$namePost\n");
    fclose($fp);
    fprintf($subfp, "qsub -cwd ./$filename \n");
    if ($namePost == 0) {
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep score > $outputdir/$datafile_score\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep degree > $outputdir/$datafile_degree\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep third > $outputdir/$datafile_third\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep hybrid > $outputdir/$datafile_hybrid\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep -P '^mass' > $outputdir/$datafile_mass\n");
    }
    else {
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep score >> $outputdir/$datafile_score\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep degree >> $outputdir/$datafile_degree\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep third >> $outputdir/$datafile_third\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep hybrid >> $outputdir/$datafile_hybrid\n");
      fprintf($dataprocessfp, "cat $outputdir/$filenamePre$namePost |grep -P '^mass' >> $outputdir/$datafile_mass\n");
    }
    $namePost += $stepLen*$stepNum;
    ++$mass_hybrid_num;
  }
  fclose($subfp);

  if ($filenamePre == 'netflix') {
    $best_lambda = 0.17;
    $best_rank = 0.058947;
    $best_precision = 0.067791;
    $best_intrasimilarity = 0.339935;
    $best_hammingd = 0.555590;
    $best_popularity = 827.63;
    $best_score = 3.660196;

    $mass_rank = 0.060601;
    $mass_precision = 0.067722;
    $mass_intrasimilarity = 0.339877;
    $mass_hammingd = 0.555959;
    $mass_popularity = 827.64;
    $mass_score = 3.66041;
  }
  else if ($filenamePre == 'rym') {
    $best_lambda = 0.23;
    $best_rank = 0.057259;
    $best_precision = 0.040551;
    $best_intrasimilarity = 0.126140;
    $best_hammingd = 0.923887;
    $best_popularity = 608.25;
    $best_score = 7.559685;
    
    $mass_rank = 0.057259;
    $mass_precision = 0.040551;
    $mass_intrasimilarity = 0.126140;
    $mass_hammingd = 0.923887;
    $mass_popularity = 608.25;
    $mass_score = 7.559685;
  }
  else {
  // ($filenamePre == 'movielens') {
    $best_lambda = 0.14;
    $best_rank = 0.0908532;
    $best_precision = 0.0981175;
    $best_intrasimilarity = 0.314915;
    $best_hammingd = 0.847272;
    $best_popularity = 171.737;
    $best_score = 3.58882;

    $mass_rank = 0.115905;
    $mass_precision = 0.0848983;
    $mass_intrasimilarity = 0.381755;
    $mass_hammingd = 0.641915;
    $mass_popularity = 254.663;
    $mass_score = 3.73539;

  }

  fprintf($dataprocessfp, "gnuplot << EOF

set terminal postscript eps

set xlabel 'theta'
set ylabel 'top L=50 score average'
set output '$outputdir/averagescore_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:17 w lp pt 19 ps 0.2 lc 3 title 'averagescore_score', '$outputdir/$datafile_degree' using 3:17 w lp pt 19 ps 0.2 lc 4 title 'averagescore_degree', '$outputdir/$datafile_third' using 3:17 w lp pt 19 ps 0.2 lc 1 title 'averagescore_third', $best_score with points pt 18 ps 0.2 title 'best hybrid $best_lambda', $mass_score with points pt 10 lc 20 ps 0.2 title 'mass'
set output

set xlabel 'theta'
set ylabel 'RankScore'
set output '$outputdir/RankScore_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:7 w lp pt 19 ps 0.2 lc 3 title 'RankScore_score', '$outputdir/$datafile_degree' using 3:7 w lp pt 19 ps 0.2 lc 4 title 'RankScore_degree', '$outputdir/$datafile_third' using 3:7 w lp pt 19 ps 0.2 lc 1 title 'RankScore_third', $best_rank with points pt 18 ps 0.2 title 'best hybrid $best_lambda', $mass_rank with points pt 10 lc 20 ps 0.2 title 'mass'
set output

set xlabel 'theta'
set ylabel 'Precision'
set output '$outputdir/Precision_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:9 w lp pt 19 ps 0.2 lc 3 title 'Precision_score', '$outputdir/$datafile_degree' using 3:9 w lp pt 19 ps 0.2 lc 4 title 'Precision_degree', '$outputdir/$datafile_third' using 3:9 w lp pt 19 ps 0.2 lc 1 title 'Precision_third', $best_precision with points pt 18 ps 0.2 title 'best hybrid $best_lambda', $mass_precision with points pt 10 lc 20 ps 0.2 title 'mass'
set output

set xlabel 'theta'
set ylabel 'Intrasimilarity'
set output '$outputdir/Intrasimilarity_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:11 w lp pt 19 ps 0.2 lc 3 title 'Intrasimilarity_score', '$outputdir/$datafile_degree' using 3:11 w lp pt 19 ps 0.2 lc 4 title 'Intrasimilarity_degree', '$outputdir/$datafile_third' using 3:11 w lp pt 19 ps 0.2 lc 1 title 'Intrasimilarity_third', $best_intrasimilarity with points pt 18 ps 0.2 title 'best hybrid $best_lambda', $mass_intrasimilarity with points pt 10 lc 20 ps 0.2 title 'mass'
set output

set xlabel 'theta'
set ylabel 'HammingD'
set output '$outputdir/HammingD_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:13 w lp pt 19 ps 0.2 lc 3 title 'HammingD_score', '$outputdir/$datafile_degree' using 3:13 w lp pt 19 ps 0.2 lc 4 title 'HammingD_degree', '$outputdir/$datafile_third' using 3:13 w lp pt 19 ps 0.2 lc 1 title 'HammingD_third', $best_hammingd with points pt 18 ps 0.2 title 'best hybrid $best_lambda', $mass_hammingd with points pt 10 lc 20 ps 0.2 title 'mass'
set output

set xlabel 'theta'
set ylabel 'Popularity'
set output '$outputdir/Popularity_3kinds_$filenamePre.eps'
plot '$outputdir/$datafile_score' using 3:15 w lp pt 19 ps 0.2 lc 3 title 'Popularity_score', '$outputdir/$datafile_degree' using 3:15 w lp pt 19 ps 0.2 lc 4 title 'Popularity_degree', '$outputdir/$datafile_third' using 3:15 w lp pt 19 ps 0.2 lc 1 title 'Popularity_third', $best_popularity with points pt 18 ps 0.2 title 'best hybrid $best_lambda', $mass_popularity with points pt 10 lc 20 ps 0.2 title 'mass'
set output

EOF
");



      fprintf($dataprocessfp, "echo '\naverge:\nloopNum: ' > $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$3} END {print s}' $outputdir/$datafile_mass >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'R:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$5/$mass_hybrid_num} END {print s}' $outputdir/$datafile_mass  >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'PL:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$7/$mass_hybrid_num} END {print s}' $outputdir/$datafile_mass  >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'IL:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$9/$mass_hybrid_num} END {print s}' $outputdir/$datafile_mass  >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'HL:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$11/$mass_hybrid_num} END {print s}' $outputdir/$datafile_mass >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'NL:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$13/$mass_hybrid_num} END {print s}' $outputdir/$datafile_mass >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'Score:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$15/$mass_hybrid_num} END {print s}' $outputdir/$datafile_mass >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "cat $outputdir/tmp >> $outputdir/$datafile_mass\n");
      fprintf($dataprocessfp, "rm $outputdir/tmp\n");

      fprintf($dataprocessfp, "echo '\naverge:\nlambda: ' > $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$3/$mass_hybrid_num} END {print s}' $outputdir/$datafile_hybrid >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'loopNum:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$5} END {print s}' $outputdir/$datafile_hybrid >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'R:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$7/$mass_hybrid_num} END {print s}' $outputdir/$datafile_hybrid >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'PL:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$9/$mass_hybrid_num} END {print s}' $outputdir/$datafile_hybrid >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'IL:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$11/$mass_hybrid_num} END {print s}' $outputdir/$datafile_hybrid >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'HL:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$13/$mass_hybrid_num} END {print s}' $outputdir/$datafile_hybrid >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'NL:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$15/$mass_hybrid_num} END {print s}' $outputdir/$datafile_hybrid >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "echo 'Score:' >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "awk '{s+=$17/$mass_hybrid_num} END {print s}' $outputdir/$datafile_hybrid >> $outputdir/tmp\n");
      fprintf($dataprocessfp, "cat $outputdir/tmp >> $outputdir/$datafile_hybrid\n");
      fprintf($dataprocessfp, "rm $outputdir/tmp\n");
  fclose($dataprocessfp);

  //php generateScripts.php movielens 0.02 5 2.01 data/movielens/movielens_3c 200 scripts_movielens 5 0.14
  //php generateScripts.php netflix 0.02 5 2.01 data/netflix/netflix_3c 50 scripts_netflix 5 0.17
  //php generateScripts.php rym 0.02 5 2.01 data/rym/rym_3c 20 scripts_rym 10 0.23
  

?>
