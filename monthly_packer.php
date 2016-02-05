<?php

$start_year = "2016";
$start_month = "01";
$next_month = "01";

while($start_month == $next_month){
  $year = "2016";
  $month = "01";
//  $day = "10";

  $srcdir = "/var/www/html/battleship/files/$year/$month/*/*.zip";

  foreach(glob($srcdir) as $file){

    $base = basename($file);
    $fromfile = $file;
    $fromfile = str_replace('(', '\\(', $fromfile);
    $fromfile = str_replace(")", '\\)', $fromfile);
    echo $fromfile . "\n";

    $zipdir = "/var/www/html/battleship/zips/$year/$month/";
    mkdir($zipdir, 0777, true);

    exec("unzip -p $fromfile > $zipdir${base}.txt");
    $tofile = "base";
    $tofile = str_replace(".txt", "", $tofile);
    $tofile = str_replace("(", "_", $tofile);
    $tofile = str_replace(")", "_", $tofile);
    exec("eqhex2gal70 $zipdir${base}.txt $zipdir${tofile}_b70.csv");
    echo $tofile . "\n";
  }

  break;
}


?>
