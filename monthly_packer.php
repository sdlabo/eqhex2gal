<?php

if($argc != 3){
  echo "usage: %s [year] [month]\n";
  exit(-1);
}

$year = sprintf("%04d", $argv[1]);
$month= sprintf("%02d", $argv[2]);

$zipdir = "/var/www/html/battleship/zips/$year/$month/";
mkdir($zipdir, 0777, true);

$pattern = "/var/www/html/battleship/files/$year/$month/*/*.zip";

foreach(glob($pattern) as $file){

  $base = basename($file);

  $file = str_replace('(', '\\(', $file);
  $file = str_replace(")", '\\)', $file);

  $fromfile = $file;
  echo $fromfile . "\n";


  $base = str_replace("(", "_", $base);
  $base = str_replace(")", "_", $base);

  echo "calling unzip\n";
  exec("unzip -p $fromfile > $zipdir${base}.txt");
  $tofile = "$base";
  $tofile = str_replace(".txt", "", $tofile);

  echo "calling exhex2gal70\n";
  exec("eqhex2gal70 $zipdir${base}.txt $zipdir${tofile}_b70.csv");
  unlink("$zipdir${base}.txt");
  echo $tofile . "\n";
}

exec("cd $zipdir; zip -o $year${month}_b70.zip *.csv");
exec("rm $zipdir/*.csv");

?>
