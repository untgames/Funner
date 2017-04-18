#!/bin/sh
echo Results of iconv01_test:
for name in CP1255 CP1258 ISO-2022-CN-EXT ISO-2022-CN ISO-2022-JP-1 ISO-2022-JP-2 ISO-2022-JP ISO-2022-KR UTF-16BE UTF-16LE UTF-32 UTF-32BE UTF-32LE UTF-7
do
  printf "Test $name... "
  iconv -f $name -t UTF-8 < data/$name-snippet > $name.tmp-snippet
  diff --strip-trailing-cr data/$name-snippet.UTF-8 $name.tmp-snippet || exit 1
  rm $name.tmp-snippet
  echo Ok
done
